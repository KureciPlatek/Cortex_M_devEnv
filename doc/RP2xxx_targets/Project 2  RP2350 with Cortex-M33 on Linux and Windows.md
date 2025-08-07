
all install and setup is quite easy.

## CMake config

Use CMakeLists.txt file as simple as possible.

- Install pico-sdk with github clone of repo
- Do the submodule init & install
- Do not manually add include of pico-sdk to your project but call target_link_libraries() to pico_stdlib
- call pico_sdk_init()
- Set PICO_BOARD to pico2 for RP2350

No need to configure UART for printf() it is done automatically it seems

Download picotool and put it somewhere you like. Download: [Releases · raspberrypi/pico-sdk-tools](https://github.com/raspberrypi/pico-sdk-tools/releases) Then I installed it in /opt/ on Linux and gave the path to CMake with:
`set(picotool_DIR /opt/picotool)`

### OpenOCD config to flash & debug with RP Debug probe

After an apt-get upgrade of openocd, I did got all rp2350 config file. Executing 
```bash
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000"
```
Did worked perfectly.

### Blink
I had to set PICO_BOARD to pico2_w and link project with pico_cyw43_arch_none as pico board must go through CYW43 Bluetooth + WiFi chip to control LED on PICO2W board

## ThreadX on RP2350 PICO2_W board

Use of azure-rtos from STMicroelectronics provided git repo but should be fine

Change of tx_initialize_low_level.S file to use the one provided in cortex_m33/ folder instead of cortex_m0 for RP2040

#### Adaptation of tx_initialize_low_level.S

#### System clock
Have to check `SYSTEM_CLOCK`. In `pico-sdk/src/rp2_common/hardware_clocks/include/hardware/clocks.h`, there is the following comment for RP2350:
```c
// Default PLL configuration RP2350:
//                   REF     FBDIV VCO            POSTDIV
// PLL SYS: 12 / 1 = 12MHz * 125 = 1500MHz / 5 / 2 = 150MHz
// PLL USB: 12 / 1 = 12MHz * 100 = 1200MHz / 5 / 5 =  48MHz
```

Which specifies the two main clocks of system. So we run at 150000000 Hz.

#### RAM and dynamic allocation
Specificity of `__RAM_segment_used_end__` It is only used in case of dynamic allocation for ThreadX. If defined with `#define USE_DYNAMIC_MEMORY_ALLOCATION`, then it is important to add the following lines in linker file (for GNU tool-chain only):
```ld
       ._threadx_heap :
       {
        . = ALIGN(8);
        __RAM_segment_used_end__ = .;
        . = . + 64K;
        . = ALIGN(8);
       } >RAM_D1 AT> RAM_D1
```

#### Setup stack and heap sizes
```asm
STACK_SIZE          =     0x00000400
HEAP_SIZE           =     0x00000000
```

#### IRQ handlers
##### UsageFault_Handler

Is new, compared to tx_initialize_low_level.S of stm32h7 project

##### Other handlers
Check if all IRQ handler are required:
`__tx_BadHandler`
`__tx_IntHandler`
`SysTick_Handler`
`HardFault_Handler`
`UsageFault_Handler` -> new compared to H7
`__tx_NMIHandler`
`__tx_DBGHandler`

Otherwise all irq are declared and defined in both cortex-M architectures. 

### Error while linking

I had the error:

```bash
[100%] Linking CXX executable prj_rp.elf
/usr/lib/gcc/.../arm-none-eabi/bin/ld: no address assigned to the veneers output section .gnu.sgstubs
```
Which slides all sections at linking and generates many warnings:
```bash
/usr/lib/gcc/.../arm-none-eabi/bin/ld: prj_rp.elf: warning: allocated section `.text' not in segment
/usr/lib/gcc/.../arm-none-eabi/bin/ld: prj_rp.elf: warning: allocated section `.rodata' not in segment
/usr/lib/gcc/.../arm-none-eabi/bin/ld: prj_rp.elf: warning: allocated section `.ARM.exidx' not in segment
/usr/lib/gcc/.../arm-none-eabi/bin/ld: prj_rp.elf: warning: allocated section `.binary_info' not in segment
/usr/lib/gcc/.../arm-none-eabi/bin/ld: prj_rp.elf: warning: allocated section `.data' not in segment
/usr/lib/gcc/.../arm-none-eabi/bin/ld: prj_rp.elf: warning: allocated section `.flash_end' not in segment
```

Citing ARM:

> In Arm GNU toolchain, the linker allocates the veneer code of Secure APIs in the .gnu.sgstubs section. You must place this section in the Secure Non-secure Callable (NSC) region in the GCC linker script .ld file. The following snippet shows you how to place veneer section in the NSC region. You should set __STACKSEAL_SIZE to 8 in the linker script to reserve the memory space for secure stack sealing.

Which means we have to add the following lines in linker file:
```ld
__ROM_BASE_NSC =  0x101FFC00;    
__ROM_SIZE_NSC =  0x400; 

MEMORY
{
    FLASH_NSC (rx)  : ORIGIN = __ROM_BASE_NSC, LENGTH = __ROM_SIZE_NSC
}

.gnu.sgstubs :
{
    . = ALIGN(32);
    *(.gnu.sgstubs*)   
} > FLASH_NSC
```

To add .gnu.sgstubs in Secure Non-secure Callable (NSC) SRAM section

How to hack it all baby:

in memmap_default.ld, whe have:
```ld
MEMORY
{
    INCLUDE "pico_flash_region.ld"
    RAM(rwx) : ORIGIN =  0x20000000, LENGTH = 512k
    SCRATCH_X(rwx) : ORIGIN = 0x20080000, LENGTH = 4k
    SCRATCH_Y(rwx) : ORIGIN = 0x20081000, LENGTH = 4k
}
```

So in "pico_flash_region.ld" we are gonna add some stuff

-> Give space to a memory for secure action of 8K as demanded by ARM

So I did in the ./bin/pico_flash_region.ld is add:

```ld
FLASH(rx) : ORIGIN = 0x10000000, LENGTH = (4 * 1024 * 1024) - 8192
FLASH_NSC(rx)  : ORIGIN = 0x103FE000, LENGTH = 8K
```
And add on default memory map manager linker file (memmap.ld):
```
    .gnu.sgstubs : {
       . = ALIGN(4);
       *(.gnu.sgstubs*)   /* Secure Gateway stubs */
       . = ALIGN(4);
    } >FLASH_NSC
```
And it links.
But errors at startup are not good

### tx_user.h

Take tx_user_sample.h from azureRTOSThreadx/common/include/ and rename it tx_user.h. Is arch independent and configures only ThreadX common files (behavior)


## Debug

svd file may be found in sdk-pico, at src/rp2350/hardware_regs/rp2350.svd

