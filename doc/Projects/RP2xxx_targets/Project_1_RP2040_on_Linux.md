
>[!ABSTRACT]
>In this section, we will look how to run a simple `ThreadX` program on a pico baord with an `RP2040`, but we will run it with the following debuggers:
>- Its supported Raspberry Debug Probe
>- SEGGER's JLink EDU min
>- Black Magic Debug Probe

Port `ThreadX` on `RP2040` was actually, like for stm23f4 project, not very complicated. Some difficulties were encountered, but with the help of other projects example, it became quite easy.

### Initialize `ThreadX` low level:

I encountered some difficulties on official `ThreadX`'s `tx_initialize_low_level.S` and had to modify following lines:

Deactivate `.global  __RAM_segment_used_end__`:
```diff
-    .global  __RAM_segment_used_end__
+@    .global  __RAM_segment_used_end__
```

Modify vector table to be in ram:
```diff
-    .global  _vectors
+@    .global  _vectors
+    .global  ram_vector_table
```

And deactivate  `Set base of available memory to end of non-initialised RAM area.`:

```diff
-    LDR     r0, =_tx_initialize_unused_memory       @ Build address of unused memory pointer
-    LDR     r1, =__RAM_segment_used_end__           @ Build first free address
-    ADDS    r1, r1, #4                              @
-    STR     r1, [r0]                                @ Setup first unused memory pointer
+@    LDR     r0, =_tx_initialize_unused_memory       @ Build address of unused memory pointer
+@    LDR     r1, =__RAM_segment_used_end__           @ Build first free address
+@    ADDS    r1, r1, #4                              @
+@    STR     r1, [r0]                                @ Setup first unused memory pointer
```


### Configure `openOCD` scripts

The `rp2040.cfg` configuration script from official `openOCD` repository seemed not to work so I took the configure script provided when installing pico's extension plugin on VSCode.

But maybe it was just because my pico board was not powered anymore and had to re-plug it. Who knows, now it works :)

### Modifications in source code and `CMake`:

In `app/app_thread.c` and `app/app_thread.h`, almost no modifications (actually no differences with projects `prj_h7` and `prj_f4`)

`system/tx_initialize_low_level.c` was as explained, modified to be for `cortex-m0`

In `CMakeLists.txt`, some modifications are important:
- Add `cmsis_core` in `target_link_library()`
- Set RTOS variable to `ThreadX` of course:
```CMake
# --- Selection of RTOS
set (RTOS_THREADX 1)   # Selection of RTOS, choose between RTOS_THREADX and RTOS_FREERTOS
```
- Add `ThreadX` sources manually as we are not using the integrated `CMakeLists.txt` of `ThreadX` but directly building and linking source to our project:
```CMake
set (PROJECT_SOURCES 
					 ...
                     ${CMAKE_SOURCE_DIR}/app/app_threadx.c
                     ${CMAKE_SOURCE_DIR}/system/tx_initialize_low_level.S
                     ${THREADX_PROJECT_SOURCES}
					 ...
)
```
- In `cortex_m_rtos.cmake`: `tx_misra.S` do not exists for port on Cortex-M0+ of `ThreadX`: So comment it out in `THREADX_PORT_SRC`:
```CMake
# ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_misra.S
```

# Debug with Raspberry Debug Probe

### With `openOCD` as `gdb-server` and `gdb` in terminal


```bash
# In one terminal, start openOCD as gdb-server
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000"

# Expected result:
Open On-Chip Debugger 0.12.0+dev-snapshot (2025-07-16-14:15)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
Info : [rp2040.core0] Hardware thread awareness created
...
Info : SWD DPIDR 0x0bc12477, DLPIDR 0x10000001
Info : [rp2040.core0] Cortex-M0+ r0p1 processor detected
Info : [rp2040.core0] target has 4 breakpoints, 2 watchpoints
Info : [rp2040.core0] Examination succeed
Info : [rp2040.core1] Cortex-M0+ r0p1 processor detected
Info : [rp2040.core1] target has 4 breakpoints, 2 watchpoints
Info : [rp2040.core1] Examination succeed
Info : [rp2040.core0] starting gdb server on 3333
Info : Listening on port 3333 for gdb connections

# When a gdb session connects to this server:
Info : accepting 'gdb' connection on tcp/3333
Info : RP2040 rev 2, QSPI Flash win w25q16jv id = 0x1540ef size = 2048 KiB in 512 sectors
Info : New GDB Connection: 1, Target rp2040.core0, state: halted
[rp2040.core0] halted due to debug-request, current mode: Thread
xPSR: 0xf1000000 pc: 0x000000ea msp: 0x20041f00
...
[rp2040.core0] halted due to debug-request, current mode: Thread
xPSR: 0xf1000000 pc: 0x000000ea msp: 0x20041f00
[rp2040.core1] halted due to debug-request, current mode: Thread
xPSR: 0xf1000000 pc: 0x000000ea msp: 0x20041f00
```


```bash
# In the other, connect to gdb-server with gdb (-iex "add-auto-load-safe-path ./" will load and run the .gdbinit file)
arm-none-eabi-gdb -iex "add-auto-load-safe-path ./" ./bin/*.elf

# Expected result prompted:
GNU gdb (Arm GNU Toolchain 14.3.Rel1 (Build arm-14.174)) 15.2.90.20241229-git
Copyright (C) 2024 Free Software Foundation, Inc.
...
Reading symbols from ./bin/prj_rp.elf...
0x000000ea in ?? ()
Loading section .boot2, size 0x100 lma 0x10000000
Loading section .text, size 0x3964 lma 0x10000100
Loading section .rodata, size 0x258 lma 0x10003a64
Loading section .binary_info, size 0x28 lma 0x10003cbc
Loading section .data, size 0x180 lma 0x10003ce4
Start address 0x100001e8, load size 15972
Transfer rate: 9 KB/sec, 3194 bytes/write.
(gdb) # Here you are ready to set breakpoints, look at backtrace, observe memory, run and so on

```

Works fine, no problem.

### Configure VSCode to use Raspberry Debug Probe as `gdb-server`

I configured again the Marius's extension for Cortex-M debug. I slightly modified the one I explained in file [[About_the_Pico-SDK]] where I explain how to configure VSCode for debug

```json
   "configurations": [
         {
         "name": "prj_rp2040_debug_openOcd",
         "cwd": "${workspaceRoot}",
         "executable": "${workspaceRoot}/prj_rp2040/bin/prj_rp.elf",
         "request": "launch",
         "type": "cortex-debug",
         "servertype": "external",
         "gdbTarget": "localhost:3333",
         "svdFile": "${workspaceFolder}/../../pico-sdk/src/rp2040/hardware_regs/RP2040.svd",
         "runToEntryPoint": "main",
         "overrideLaunchCommands": [
             "monitor reset init"
         ]
      },
      {
	     ... /* other configs */
      }      
```


>[!HINT]
>Keep in mind that it is for Raspberry Debug Probe, with `openOCD` as `gdb-server`
>For other debuggers, it will be different. Good luck with `launch.json` configuration.

**Result on VSCode**:

![[debug_example_VSCode_rp2040.png]]

All works fine, I can run the project and see the beautiful blink.


# Debug with J-Link EDU

For a full understanding of JLink EDU mini, please look at the documentation website of Segger: [UM08001 J-Link / J-Trace User Guide - SEGGER Knowledge Base](https://kb.segger.com/UM08001_J-Link_/_J-Trace_User_Guide)

Then, to understand the interface between `JLink` and the target, please look at: `JLink` Connector: [9-pin JTAG/SWD connector - SEGGER Knowledge Base](https://kb.segger.com/9-pin_JTAG/SWD_connector)

Where the connector is like:
![[JLink_EDU_mini_9_PinConnector_Port.png]]
We will have to do some mechanical adaptation to connect this interface to the pico board target:

| Pin on JLink EDU min                                                                                                     | Pin on Pico board                  |
| ------------------------------------------------------------------------------------------------------------------------ | ---------------------------------- |
| Pin 1: `VTref` (target reference voltage)                                                                                | Pin 36: `3V3 Vout`                 |
| Pin 2: SWDIO (SWD Input Output)                                                                                          | Debug/`SWDIO` (at bottom of board) |
| Pin 3: GND                                                                                                               | To any GND pin of pico board       |
| Pin 4: SWCLK (SWD clock)                                                                                                 | Debug/`SWCLK` (at bottom of board) |
| Pin 5: NC                                                                                                                | X                                  |
| Pin 6: SWO, not connected as Pico/RP2040 doesn't support Serial Wire Output/Trace. Look at [[#From Raspberry Engineers]] | X                                  |
| Pin 7: NC                                                                                                                | X                                  |
| Pin 8: TDI, JTAG specific, not available on Pico/RP2040 board                                                            | X                                  |
| Pin 9: NC                                                                                                                | X                                  |
| Pin 10: nRESET, negative reset, is actually not mandatory but it still is useful                                         | Pin 30: `RUN`                      |

###### From Raspberry Engineers:

> The description applies when using the M33 specific TPIU - a low cost trace solution, that lacks many of the features of a full Coresight 600M system. We use the 600M TPIU, which doesn't support the TRACESWO output.
> 
> We don't have SWO. Trace output goes through the TPIU, which has a 4-bit DDR output (much faster than SWO) or to the trace FIFO. I'll add a note to the generic Arm text in the datasheet.

From [Pico2 and SWO - Raspberry Pi Forums](https://forums.raspberrypi.com/viewtopic.php?t=375494)


### With `openOCD` and `gdb`

Simple:

```bash
# run openOCD:
sudo openocd -f interface/jlink.cfg -f target/rp2040.cfg -c "transport select swd" -c "adapter speed 5000"

# Expected result when waiting for gdb connection:
Open On-Chip Debugger 0.12.0+dev-snapshot (2025-07-16-14:15)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
Info : [rp2040.core0] Hardware thread awareness created
Info : [rp2040.core1] Hardware thread awareness created
Warn : Transport "swd" was already selected
adapter speed: 5000 kHz
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : J-Link EDU Mini V1 compiled Jun  9 2020 13:36:46
Info : Hardware version: 1.00
Info : VTarget = 3.282 V
Info : Reduced speed from 5000 kHz to 4000 kHz (maximum)
Info : Reduced speed from 5000 kHz to 4000 kHz (maximum)
Info : clock speed 5000 kHz
Info : SWD DPIDR 0x0bc12477, DLPIDR 0x00000001
Info : SWD DPIDR 0x0bc12477, DLPIDR 0x10000001
Info : [rp2040.core0] Cortex-M0+ r0p1 processor detected
Info : [rp2040.core0] target has 4 breakpoints, 2 watchpoints
Info : [rp2040.core0] Examination succeed
Info : [rp2040.core1] Cortex-M0+ r0p1 processor detected
Info : [rp2040.core1] target has 4 breakpoints, 2 watchpoints
Info : [rp2040.core1] Examination succeed
Info : [rp2040.core0] starting gdb server on 3333
Info : Listening on port 3333 for gdb connections
Info : accepting 'gdb' connection on tcp/3333

# When gdb connected to it:
Info : RP2040 rev 2, QSPI Flash win w25q16jv id = 0x1540ef size = 2048 KiB in 512 sectors
Info : New GDB Connection: 1, Target rp2040.core0, state: halted
[rp2040.core0] halted due to breakpoint, current mode: Thread
xPSR: 0x61000000 pc: 0x000020e0 msp: 0x50100898
[rp2040.core1] halted due to debug-request, current mode: Thread
xPSR: 0x01000000 pc: 0x00000184 msp: 0x20041f00
[rp2040.core0] halted due to debug-request, current mode: Thread
xPSR: 0xf1000000 pc: 0x000000ea msp: 0x20041f00
[rp2040.core1] halted due to debug-request, current mode: Thread
xPSR: 0xf1000000 pc: 0x000000ea msp: 0x20041f00
Error: Invalid command argument
Error: [rp2040.core0] Execution of event reset-init failed:
embedded:startup.tcl:1427: Error: args[i] option value ('CX') is not valid
in procedure 'ocd_process_reset'
in procedure 'ocd_process_reset_inner' called at file "embedded:startup.tcl", line 1427
Info : Padding image section 0 at 0x10003e64 with 156 bytes (bank write end alignment)
[rp2040.core0] halted due to debug-request, current mode: Thread
xPSR: 0xf1000000 pc: 0x000000ea msp: 0x20041f00
[rp2040.core1] halted due to debug-request, current mode: Thread
xPSR: 0xf1000000 pc: 0x000000ea msp: 0x20041f00
[rp2040.core0] halted due to debug-request, current mode: Thread
xPSR: 0xf1000000 pc: 0x000000ea msp: 0x20041f00
[rp2040.core1] halted due to debug-request, current mode: Thread
xPSR: 0xf1000000 pc: 0x000000ea msp: 0x20041f00
```

Run, flash, break and so on work perfectly fine

On gdb, I could perfectly run, stop and read memory:

```bash
# Read registers
(gdb) info registers
r0             0x7735940           125000000
r1             0x10000321          268436257
r2             0xe000e404          -536812540
r3             0x1                 1
r4             0x10000268          268436072
r5             0x20041f01          537140993
r6             0x18000000          402653184
r7             0x0                 0
r8             0xffffffff          -1
r9             0xffffffff          -1
r10            0xffffffff          -1
r11            0xffffffff          -1
r12            0x34000040          872415296
sp             0x20042000          0x20042000
lr             0x10000227          268436007
pc             0x10000320          0x10000320 <main>
xpsr           0x61000000          1627389952
msp            0x20042000          0x20042000
psp            0xfffffffc          0xfffffffc
primask        0x0                 0
basepri        0x0                 0
faultmask      0x0                 0
control        0x0                 0
```


```bash
# Print some variables
(gdb) p/x tx_app_byte_pool

$3 = {
  tx_byte_pool_id = 0x42595445,
  tx_byte_pool_name = 0x10003b7c,
  tx_byte_pool_available = 0x580,
  tx_byte_pool_fragments = 0x7,
  tx_byte_pool_list = 0x20000f48,
  tx_byte_pool_search = 0x200015c0,
  tx_byte_pool_start = 0x20000f48,
  tx_byte_pool_size = 0xc00,
  tx_byte_pool_owner = 0x0,
  tx_byte_pool_suspension_list = 0x0,
  tx_byte_pool_suspended_count = 0x0,
  tx_byte_pool_created_next = 0x20000e24,
  tx_byte_pool_created_previous = 0x20000e24
}
```


### With `JLink GDB server` and `gdb`

I launched `JLinkGDBServerCLExe` and could connect to target easily with `gdb`. The command line was a little bit tricky, but I ran first JLinkGDBServerExe to have a GUI and configure the parameters to provide to :

```bash
# Call JLink gdb-server using USB connected device in local:
JLinkGDBServerCLExe -USB 0 -device RP2040_M0_0 -endian little -if SWD -speed auto -ir -LocalhostOnly -nologtofile -port 3333 -SWOPort 2332 -TelnetPort 2333

# Output:
SEGGER J-Link GDB Server V8.74a Command Line Version

JLinkARM.dll V8.74a (DLL compiled Oct  2 2025 18:31:08)

Command line: -USB 0 -device RP2040_M0_0 -endian little -if SWD -speed auto -ir -LocalhostOnly -nologtofile -port 3333 -SWOPort 2332 -TelnetPort 2333
-----GDB Server start settings-----
GDBInit file:                  none
GDB Server Listening port:     3333
SWO raw output listening port: 2332
Terminal I/O port:             2333
Accept remote connection:      localhost only
Generate logfile:              off
Verify download:               off
Init regs on start:            on
Silent mode:                   off
Single run mode:               off
Target connection timeout:     0 ms
------J-Link related settings------
J-Link Host interface:         USB
J-Link script:                 none
J-Link settings file:          none
------Target related settings------
Target device:                 RP2040_M0_0
Target device parameters:      none
Target interface:              SWD
Target interface speed:        auto
Target endian:                 little

Connecting to J-Link...
J-Link is connected.
Firmware: J-Link EDU Mini V1 compiled Sep 17 2025 12:04:20
Hardware: V1.00
S/N: 801022472
Feature(s): FlashBP, GDB
Checking target voltage...
Target voltage: 3.28 V
Listening on TCP/IP port 3333
Connecting to target...
Initializing CPU registers...Connected to target
Waiting for GDB connection...

```

>[!IMPORTANT]
>It is here important to notify a couple of things:
>- `JTAG` is not supported by Pico, so I had to specify using SWD
>- `-USB 0` means use connected device USB 0, we should have only one
>- `-port 3333` I modified it to be on port 3333 because I configured all `gdb` sessions to connect on port 3333. But you may provide the one you like


Then, on another terminal, I ran my gdb command and could connect. On JLink gdb-server side I got the following output:


```bash
Connected to 0000:0000:0000:0000:0000:0000:0000:0001
GDB client (conn. 11) requested target.xml from GDB Server
Reading common registers:
Read register 'r0' (4 bytes) from hardware: 0x00000000
Read register 'r1' (4 bytes) from hardware: 0x00000000
Read register 'r2' (4 bytes) from hardware: 0x00000000
Read register 'r3' (4 bytes) from hardware: 0x00000000
Read register 'r4' (4 bytes) from hardware: 0x00000000
Read register 'r5' (4 bytes) from hardware: 0x00000000
Read register 'r6' (4 bytes) from hardware: 0x00000000
Read register 'r7' (4 bytes) from hardware: 0x00000000
Read register 'r8' (4 bytes) from hardware: 0x00000000
Read register 'r9' (4 bytes) from hardware: 0x00000000
Read register 'r10' (4 bytes) from hardware: 0x00000000
Read register 'r11' (4 bytes) from hardware: 0x00000000
Read register 'r12' (4 bytes) from hardware: 0x00000000
Read register 'sp' (4 bytes) from hardware: 0x00000000
Read register 'lr' (4 bytes) from hardware: 0x00000000
Read register 'pc' (4 bytes) from hardware: 0x00000000
Read register 'xpsr' (4 bytes) from hardware: 0x01000000
Read 4 bytes @ address 0x00000000 (Data = 0x20041F00)
Read 2 bytes @ address 0x00000000 (Data = 0x1F00)
Downloading 256 bytes @ address 0x10000000
Downloading 14692 bytes @ address 0x10000100
Downloading 600 bytes @ address 0x10003A64
Downloading 40 bytes @ address 0x10003CBC
Downloading 384 bytes @ address 0x10003CE4
Writing register 'pc' = 0x100001E8
```

Which looks quite fine


### With `JLink GDB server` and `Ozone`

Sadly, Ozone is not yet supported on Linux ARM, BUT! It is possible to connect to a remote target through LAN, which is quite nice! Because I run Ozone from my Windows laptop and I just have to run `JLinkRemoteServer` on the Raspberry from the other side and I can also start a remote debug session:

```bash
# Run JLink remote server on remote machine (RPi5 with Linux)
JLinkRemoteServer

# Output:
SEGGER J-Link Remote Server V8.74a
Compiled Oct  2 2025 18:34:54

'q' to quit '?' for help

2025-10-07 16:35:07 - Remote Server started
2025-10-07 16:35:07 - Connected to J-Link with S/N 801022472
2025-10-07 16:35:07 - Waiting for client connections...

# After sttarting session from Ozone on local machine (Windows):
2025-10-07 16:36:11 - Conn. 9: Client connected.
2025-10-07 16:36:11 - Client connected
```

On Windows machine, select the host interface to be TCP/IP and provide the IP address of the remote machine. In my case, `10.43.2.19`:

![[ozone_rp2040_worked_remoteSession_config.png]]

Then connect (start a debug session) with Ozone:

![[ozone_rp2040_worked_remoteSession.png]]


# Debug with Black Magic Debug probe

### First setup and maybe install

Find it with `dmesg`:

```bash
sudo dmesg

# Somewhere we should find:
[    0.974487] usb 3-1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[    0.974489] usb 3-1: Product: Black Magic Probe
[    0.974491] usb 3-1: Manufacturer: Black Sphere Technologies
[    0.974493] usb 3-1: SerialNumber: E2E1B4DF
[    3.485701] cdc_acm 3-1:1.0: ttyACM0: USB ACM device
[    3.488641] cdc_acm 3-1:1.2: ttyACM1: USB ACM device
...
```

But on /dev/tty* it is possible that the device is hidden. udev rules have to be updated for that, look at: [blackmagic/driver at main · blackmagic-debug/blackmagic](https://github.com/blackmagic-debug/blackmagic/tree/main/driver#99-blackmagic-plugdevrules)

Basically, file [blackmagic/driver/99-blackmagic-plugdev.rules at main · blackmagic-debug/blackmagic](https://github.com/blackmagic-debug/blackmagic/blob/main/driver/99-blackmagic-plugdev.rules) has to be added in `/etc/udev/rules.d/`

```bash
# Add Black Magic Probe to udev rules:
sudo nano /etc/udev/rules.d/99-blackmagic-plugdev.rules

# Reload rules:
sudo udevadm control --reload-rules

# If it still does't show /dev/ttyBmpGdb then reboot target
sudo reboot
```

For more fun about configuring BMP, look at the `bmputil` installation here: [[Debuggers]] in section BMP

### Debug session

To start a debug session, it is actually not required to run openOCD or any gdb-server as it is already managed by debug probe. Just run gdb


### Add `UART`'s `printf()` to BMP handling

BMP comes with a second connector below, the PH1, which allows to put an `UART` interface on it. Quite handy, like for STLink, as we may read `printf()` forwarded on the default `UART0` of Pico's RP2040.

Indeed, I have the hardware V2.1e and its schematic here: [BMPM2](https://black-magic.org/_downloads/5a984c502435264dc19bc5e08c8b55cd/bmpm_v2_1e_schematic.pdf) shows that PH1 accepts an UART input:

![[bmp_uart_port.png]]
So I connected Rx and Tx from RP2040 to those two sexy pins:


| BMP connector PH1 | Pico board pins                 |
| ----------------- | ------------------------------- |
| TPWR (pin 1)      | Not connected (already powered) |
| TXD (pin 2)       | UART0 RX (pin 2)                |
| RXD (pin 3)       | UART0 TX (pin 1)                |
| GND (pin 4)       | GND (pin 3)                     |

Then with any serial debug monitoring like minicom:

```bash
minicom -D /dev/ttyACM1 -b 115200
```

I could see the results of `ThreadX` sending its messages through `printf()`, YAY!

### Integrate BMP to VSCode

After a **LOT OF FIGHT!!!** and I really mean it, those `launch.json` are really not what I like (even if I have to thank your for your job on Cortex-Debug plugin, Marus). I made it to get a configuration for VSCode to control, flash, read memory of an RP2040 with BMP as debug probe:

```json
      {
         "name": "prj_rp2040_debug_BMP",
         "cwd": "${workspaceRoot}",
         "executable": "${workspaceRoot}/prj_rp2040/bin/prj_rp.elf",
         "request": "attach",
         "type": "cortex-debug",
         "servertype": "bmp",
         "BMPGDBSerialPort": "/dev/ttyBmpGdbE2E1B4DF",
         "powerOverBMP" : "disable",
         "svdFile": "${workspaceFolder}/../../pico-sdk/src/rp2040/hardware_regs/RP2040.svd",
         "runToEntryPoint": "main",
         "overrideLaunchCommands": [
             "monitor swdp_scan",
             "attach 2",
             "load",
             "run"
         ]
      },
```

The important things here, are:
- Set `"servertype": "bmp"` for Black Magic Probe
- Configure `gdb` to aim for the `/dev/` of BMP: `"BMPGDBSerialPort": "/dev/ttyBmpGdbE2E1B4DF",` and remove `"gdbTarget": "localhost:3333",` It is due to the fact that BMP already includes the `gdb-server`, so instead to look for localhost, reach for the BMP device
- Add the `"overrideLaunchCommands"` to what I wrote. It is a little bit strange, but it works. I also had to attach to target 2 as it was there that I could load (flash) the program. Some `rp2xxx` specificity I guess.

>[!WARNING]
>I must say, I did got it to work. But when looking at the quality of Cortex-Debug plugin's documentation, and the way I just "try and look" at randomly configuring my `launch.json` file, I really not recommend to debug this way. I just can't explain why it works and it is a pity.
>
>There is a high chance that it won't work for you...


### Future developments

Many things like automatization are possible with this small debugger, and everything is open-source and quite well explained. Like add `make flash` to your commands in `makefile`:

```Makefile
flash: <program>.elf

BMP_PORT ?= /dev/ttyACM0

%.flash: %.elf
	@printf "  BMP $(BMP_PORT) $(*).elf (flash)\n"
	$(Q)$(GDB) -nx --batch \
	           -ex 'target extended-remote $(BMP_PORT)' \
	           -x black_magic_probe_flash.scr \
	           $(*).elf
```

And many other possibilities:
- [GDB Flash Automation — Black Magic Debug documentation](https://black-magic.org/usage/gdb-automation.html)
- [Serial Wire Output — Black Magic Debug documentation](https://black-magic.org/usage/swo.html) for tracing. Sadly not supported by `rp2xxx` family
- Have more fun with `gdb` commands: [GDB Commands — Black Magic Debug documentation](https://black-magic.org/usage/gdb-commands.html)
- [Semihosting — Black Magic Debug documentation](https://black-magic.org/usage/semihosting.html) so that system calls are forwarded to our laptop.

# Conclusion

It is always a little bit the same, have a `gdb-server` ready to listen to `gdb` connection. Still, there are as shown, huge differences and configuration time between the different configuration.

Anyway, as everything (most of it) is open, it allows to find a way to debug a program, using few tools and devices (or cheap one). Or create your own solution.

My opinion on all of that is that the less integrated/automated, the better. It is not important to have the most expensive tool, as long as we understand what is done, can easily find a solution, a fix to solve our problem.

