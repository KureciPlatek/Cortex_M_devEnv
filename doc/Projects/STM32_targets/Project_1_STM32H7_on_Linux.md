
>[!INFO] Prerequisite: 
>You have  successfully compiled a `prj_h7.elf` from project `prj_h7/` (project for stm32h723zgtx) with **`-g3` debug flag** (maximal debug level = have symbols in your .elf file)

**We will** 
1. first set all the different steps of debugging chain: prepare `gdb`, `gdb-server`, debugger's hardware...) prior to use it with VSCode.
2. Then we will configure VSCode to use this debugging environment. With a GUI it is more sexy.


**Diagram of my setup**

Before carrying on, I prefer to provide here a diagram of my setup, which connections are made, in a little bit more details. All connections are then explained later in this document. Refer to this diagram to understand what I am doing.

![[stm32h7_setup.svg]]

This setup is when using integrated `STLink` debugger of Nucleo board. When using `JLink` or `Black Magic Debug` probe, it is a little bit different.

| Element           | Description                                                                                                                                                                                                  |
| ----------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `USB STLink`      | Connected to connector `CN1` of Nucleo board. It will power the integrated `STLink` debugger, provide the serial communication forwarded by `STLink` and control `STLink` from `gdb` through `gdb-server`    |
| `USB Nucleo`      | Only to power Nucleo board and the STM32H7 SoC on it. Connected to `CN13` of Nucleo board                                                                                                                    |
| `STM32H723ZG SoC` | The target we want to program, manipulate, debug                                                                                                                                                             |
| `UART3`           | As it is hard wired on Nucleo board, from STM32H7 to `STLink`, we use the `UART3` instance, with pins `GPIO_D8` and `GPIO_D9`. It is possible to modify it, but soldering has to be involved                 |
| `SWD`             | Debug interface of STM32H7, part of CoreSight of ARM Cortex-M architecture. No specific configuration to be done here, except if you want to use the pins for another purpose, but then you are on your own. |
| `LED green`       | Like `UART3`, hard wired on Nucleo board on `GPIO_B0`                                                                                                                                                        |
| `LED red`         | Like `UART3`, hard wired on Nucleo board on `GPIO_B14`                                                                                                                                                       |
For deeper details, look at document: [STM32H7 Nucleo-144 boards (MB1364) - User manual](https://www.st.com/resource/en/user_manual/um2407-stm32h7-nucleo144-boards-mb1364-stmicroelectronics.pdf)
Or in source code of `prj_h7`.

## Start target and check

### Check presence of device

Plug your STM32H723ZG Nucleo or discovery board to your development laptop where you will run the debugging session. Be careful to have set this board (especially jumpers for STM's nucleo and discovery boards) so that STLink and target are well connected and powered.

On my case, after plugging it, I see the following devices appears on Linux:
```bash
$ ls /dev/
...
stlinkv3_
stlinkv3_0
stlinkv3_1
...
serial/by-id/ -> ../../ttyACM0
serial/by-path/ ->  ../../ttyACM0
...
```

`/dev/serial/by-path/` and `/dev/serial/by-id/` are just link to `/dev/ttyACM0` which is the virtual COM port to retrieve `putchar()` sent characters from target.

## Debug with `openOCD` as  `gdb-server`

When all previous steps are good, just call the config file from prj_h7 to start `openOCD` as a `gdb-server`:

```bash
# Call openOCD with stm32h7_debug.cfg config file
~/.../prj_h7 $ openocd -f stm32h7_debug.cfg

# The following should be prompted on terminal
Open On-Chip Debugger 0.12.0
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : STLINK V3J6M2 (API v3) VID:PID 0483:374E
Info : Target voltage: 3.284549
Info : Unable to match requested speed 1800 kHz, using 1000 kHz
Info : Unable to match requested speed 1800 kHz, using 1000 kHz
Info : clock speed 1000 kHz
Info : stlink_dap_op_connect(connect)
Info : SWD DPIDR 0x6ba02477
Info : [STM32H723ZGTx.cpu0] Cortex-M7 r1p2 processor detected
Info : [STM32H723ZGTx.cpu0] target has 8 breakpoints, 4 watchpoints
Info : gdb port disabled
Info : starting gdb server for STM32H723ZGTx.cpu0 on 3333
Info : Listening on port 3333 for gdb connections
Info : [STM32H723ZGTx.cpu0] external reset detected

```

Now it is ready to be used by `gdb` to start a debug session.

>[!NOTE]
>`gdb` may be called prior to `openOCD`, but if you try to connect to it with command 
>```
>target extended-remote :3333
>``` 
>It will not work, as no `gdb-server` is available 

## `gdb-server` with `st-util` from stlink-org

Also, if previous install and config steps are good, you could run `st-util` and see on terminal:

```bash
# Start st-util as gdb-server
$ st-util

# The following should be prompted on terminal
st-util
2025-07-11T11:14:50 INFO usb.c: Unable to match requested speed 1800 kHz, using 1000 kHz
2025-07-11T11:14:50 INFO common.c: H72x/H73x: 128 KiB SRAM, 1024 KiB flash in at least 128 KiB pages.
2025-07-11T11:14:50 INFO gdb-server.c: Listening at *:4242..
```

Which tells us that `gdb-server` from the open-source STLink server recognized the STM32H723 as target and is ready to start a debug session

>[!IMPORTANT]
>In both case, `st-util` and `openOCD` were able to detect target and which microcontroller is on it: `H72x/H73x` for `st-util` and `STM32H723ZGTx` for `openOCD`

>[!TCP Socket]
>Also, in both case, `st-util` and `openOCD` prompts on which port they will listen to incoming gdb client. **Keep this port number**. IP address is always localhost (except you want to provide it through other IP address and do some IP forwarding to start a remote session. It is not explained here).
>The port numbers:
>```bash
># For openOCD, port is printed here:
>Info : starting gdb server for STM32H723ZGTx.cpu0 on 3333
>
># For st-util, port is printed here:
>2025-07-11T11:14:50 INFO gdb-server.c: Listening at *:4242..
>```
>
>Hint: for `openOCD`, port is defined in the `config.cfg` file we used to start `openOCD` with


### Final step: run `gdb` and start have fun

Leave the terminal with `openOCD` running, open a new one and go where your `prj_h7.elf` file is waiting to be debugged.

First step is to call `gdb` with your elf file as argument. Then in the `gdb` console (seen with `(gdb)` prompt), enter the command `target extended-remote :<port>` where `<port>` is the given port in `<debug_session_name>.cfg` file, line: `set GDB_PORT 3333`. It is also prompted when starting `gdb-server` (see previous step)

#### With `arm-none-eabi-gdb`

```bash
# Start gdb
~/.../prj_h7/bin $ arm-none-eabi-gdb <project_name>.elf
GNU gdb (Arm GNU Toolchain 14.3.Rel1 (Build arm-14.174)) 15.2.90.20241229-git
Copyright (C) 2024 Free Software Foundation, Inc.
...
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from prj_h7.elf...

# Connect to gdb-server
(gdb) target extended-remote :3333
Remote debugging using :3333
Reset_Handler () at /home/jeremie/work/prj_blog/prj_h7/startup/startup_stm32h723zgtx.s:61
61        ldr   sp, =_estack      /* set stack pointer */

# Flash program:
(gdb) load
... # Print flashed addresses and some other infos

# Run program
(gdb) run
```

Now you are ready to load, run, stop, breakpoint and do everything you need to debug your program.

#### With `gdb-multiarch`

```bash
# Start gdb
~/.../prj_h7/bin $ gdb-multiarch prj_h7.elf
GNU gdb (Debian 13.1-3) 13.1
Copyright (C) 2023 Free Software Foundation, Inc.
...
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from prj_h7.elf...

# Connect to gdb-server:
(gdb) target extended-remote :3333
Remote debugging using :3333
Reset_Handler () at /home/jeremie/work/prj_blog/prj_h7/startup/startup_stm32h723zgtx.s:61
61        ldr   sp, =_estack      /* set stack pointer */

# Flash program:
(gdb) load
... # Print flashed addresses and some other infos

# Run program
(gdb) run
```

Also works fine.

>[!INFO] At this point, you may start flash debugging, but with terminal only.
>Using command `(gdb) set print pretty` and maybe some others, you may have a quite nice and efficient debug interface (on my opinion)

>[!TIP] Automatize gdb startup sequence
>Instead of always typing `target extended-remote :3333` loading etc... you may write them in a .gdbinit script and run it at beginning with:
>```bash
>arm-none-eabi-gdb -x ./.gdbinit <code_to_debug>.elf
>```
>With all commands to execute at startup written in this `.gdbinit` file. Contents of it is:
>```txt
># Connect to target
target extended-remote :3333
load
set print pretty
>```


# Debug Nucleo board with external debugger

I created a separated section for both JLink EDU min and Black Magic Probe, as many things are common, starting by the configuration of Nucleo board.

All explanations may be found here, on official STMicroelectronics website: [STM32H7 Nucleo-144 boards (MB1364) - User manual](https://www.st.com/resource/en/user_manual/um2407-stm32h7-nucleo144-boards-mb1364-stmicroelectronics.pdf) 

### Set of power supply

Nucleo board is powered by STLink V3E, which we are going to shut down. So JP2 has to be set to configuration 4: CHGR. Another power supply is possible but I wanted to keep hardware setup simple.

Connect jumper of JP2 to position 7-8 as shown:

![[project1_stm32h7_powerSupply.png]]


### Set of serial communication (`printf()` over `UART`)

It could be possible to receive `printf()`/`UART` messages with the debugger, or with an external serial interface, because the UART lines which were hard wired to STLink may be found on the MIPI10 connector, but on additional pins, which are not on my cable (He! Pity!):

![[project1_stm32h7_HSE_MCO_externalDebugger_MIPI10Connector.png]]

There is a hardware trick provided by STMicroelectronics, the MIPI10 connector is actually on pin 3 to 12, which makes 10 pins. The pins 1, 2, 13 and 14 are available, but not soldered to a header or connector. So I would have to add it myself.

The blue square represents where the flat ribbon cable is forced mechanically to be plugged.

>[!HINT]
>In reality, when connecting Black Magic Probe or JLink to the Nucleo board with the typical flat ribbon cable, **all pins from this MIPI10 connector are "shifted"**. So pin 3 from CN5 is actually pin 1 on Black Magic Probe, or JLink.  

>[!INFO]
>So I worked without `printf()`/`UART`, it was fine

>[!HINT] ARM's CoreSight Trace
>To go back to `printf()`, the advantage of STM32H723 and this connector, is that for both `JLink` and `BMP`, the `SWO` and `JTDI` are available, which allows us theoretically to Trace our code execution and become logs from `printf()` on those lines, which would greatly reduce impact of `printf()` on our code execution time (`SWD`/`JTDI` are part of ARM's CoreSight)
>
>I will try to execute trace with both `JLink` and `BMP` debuggers


### `HSE` is from `STLink` V3

Another problem is the `HSE`: High Speed External, which is the clock input from our STM32H723ZG target. This one is generated by STLink, which will be deactivated. 

There is no plug to provide from somewhere else, the HSE. For a good reason, an 8 MHz clock is not to plug like some random signal. Through a low quality header plug, forget your 8 MHz.

So we will have to switch to `HSI`: High Speed Internal in our code, so that we have a clock. LSE is 32768 kHz, very low. Even if HSI is not of best quality, it will be fine.

#### Code modifications on STM32H723 to work with external debugger

As said, we have on our STM32H7, the clock provided by STLink on the `PH0-OSC_IN` pin: the signal `STLK_MCO` (Master Control Output).

When we set STLink in high impedance 

![[project1_stm32h7_HSE_MCO_externalDebugger.png]]

So, main clock has to be HSI, I had to modify function:

```C
void SystemClock_Config(void)
{
...
#ifdef USE_STLINK_DEBUGGER
   ...
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   ...
  
#else
   ...
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
   ...
}
```

Then, structure `RCC_OscInitStruct.PLL` is almost the same for both code.
The main idea here is to run with HSW, if we debug with STLink, or with HSI if with an external. For that, preprocessor `USE_STLINK_DEBUGGER` is added in CMakeLists.txt of project:

```CMake
# --- Project preprocessors
add_compile_definitions(
   STM32H723xx
   USE_FULL_LL_DRIVER
   USE_HAL_DRIVER
#   USE_STLINK_DEBUGGER # Use HSE clock if debugger is STLink, HSI if debugger is external
)
```


## Debug with `JLink EDU mini`

Two ways: 
- Either run Ozone on your local machine, `JLinkRemoteServer` on the host and connect to host with Ozone, to a remote `JLink`. Very simple and sexy I must say.
- Or like always, run a `gdb-server` with `openOCD`, and execute debug session from remote machine, the RPi5.

### Debug with Ozone and `JLinkRemoteServer`

Context and situation of the different machines:

![[project1_stm32h7_jlink_remoteServer.svg]]

So the difference here is that my debug session is on my laptop. Not on the RPi5 which is the host for JLinkRemoteServer.

>[!WARNING]
>It means that you have to compile your elf file on your laptop, so that when loaded to Ozone and flashed on your target, Ozone will know where to point to: which source file, with which path. If compiled and RPi5, and ran from laptop, Ozone will tell you it can't find path to source code. 


```bash
# Run:
JLinkRemoteServer

# Output:
SEGGER J-Link Remote Server V8.74a
Compiled Oct  2 2025 18:34:54

'q' to quit '?' for help

2025-10-09 12:59:56 - Remote Server started
2025-10-09 12:59:56 - Connected to J-Link with S/N 801022472
2025-10-09 12:59:56 - Waiting for client connections...
```

On Ozone, go to `Tools` -> `J-Link Settings` -> `Host Interface` then enter the IP address of the remote machine, on which target and JLink is plugged and `JLinkRemoteServer` is working:

![[project1_stm32h7_JLink_setJLinkRemoteServer.png]]

On remote machine when laptop is connected:

```bash
# When connected with Ozone, on remote machine running JLinkRemoteServer, the following output should be seen:
2025-10-09 13:03:52 - Conn. 9: Client connected.
2025-10-09 13:03:52 - Client connected
```

Finito! Now you can have fun debug the `ThreadX` example program of `prj_h7/`

### Debug on remote machine with `openOCD` as `gdb-server`

The only thing to modify is the `stm32h7_debug.cfg` file, to go fetch `interface/jlink.cfg` configuration file, and remove line `transport select "dapdirect_swd"` as it is not supported by `JLink`. No idea why, but it worked:

```tcl
...
source [find interface/jlink.cfg]
# source [find st_interface/stlink-dap.cfg] # Deactivated, be careful when debugging with STLink again

set WORKAREASIZE 0x8000

# transport select "dapdirect_swd" # Deactivated, be careful when debugging with STLink again
...
```

Start `gdb-server`:

```bash
# Run gdb-server
openocd -f ~/path/to/prj_blog/prj_h7/stm32h7_debug.cfg

# Result:
Open On-Chip Debugger 0.12.0+dev-snapshot (2025-07-16-14:15)
Licensed under GNU GPL v2
...
Info : [STM32H723ZGTx.cm7] Examination succeed
Info : [STM32H723ZGTx.ap2] gdb port disabled
Info : [STM32H723ZGTx.cm7] starting gdb server on 3333
Info : Listening on port 3333 for gdb connections
Info : [STM32H723ZGTx.cm7] external reset detected

# When gdb connected:
...
Info : JTAG tap: STM32H723ZGTx.cpu tap/device found: 0x6ba00477 (mfg: 0x23b (ARM Ltd), part: 0xba00, ver: 0x6)
Info : JTAG tap: auto0.tap tap/device found: 0x06483041 (mfg: 0x020 (STMicroelectronics), part: 0x6483, ver: 0x0)
[STM32H723ZGTx.cm7] halted due to debug-request, current mode: Thread
xPSR: 0x01000000 pc: 0x0801ce2c msp: 0x24050000
Info : Reduced speed from 8000 kHz to 4000 kHz (maximum)
Info : Padding image section 0 at 0x080002cc with 4 bytes
Info : Padding image section 1 at 0x0801eda4 with 28 bytes (bank write end alignment)
...
```

On another terminal, run `gdb`:

```bash
# Run gdb
arm-none-eabi-gdb -iex "add-auto-load-safe-path ./" ~/path/to/prj_blog/prj_h7/bin/<prog>.elf

# .gdbinit file if exists is ran:
...
Reading symbols from ./bin/prj_h7.elf...
0x0801ce2c in __udivmoddi4 (n=0, d=0, rp=0x0) at ../../../../../../libgcc/libgcc2.c:1180
warning: 1180   ../../../../../../libgcc/libgcc2.c: No such file or directory
Loading section .isr_vector, size 0x2cc lma 0x8000000
Loading section .text, size 0x1e7dc lma 0x80002d0
Loading section .rodata, size 0x21c lma 0x801eaac
Loading section .ARM, size 0x8 lma 0x801ecc8
Loading section .init_array, size 0x4 lma 0x801ecd0
Loading section .fini_array, size 0x4 lma 0x801ecd4
Loading section .data, size 0xcc lma 0x801ecd8
Start address 0x0801ea3c, load size 126368
Transfer rate: 17 KB/sec, 9026 bytes/write.

# Run target
(gdb) run
```

Worked perfectly fine.

## Debug with Black Magic Probe (BMP)

#todo

# Look at `printf()` from project

I modified `printf()` function to redirect each character print, to an UART communication, actually the default one, which is hardware-ly connected to `STLink` debugger on Nucleo board: the UART3. 

```c
/* STEP 1 - printf() will call at some point _write() which is defined below: */

__attribute__((weak)) int _write(int file, char *ptr, int len)
{
...
    __io_putchar(*ptr++);
...
}

/* STEP 2 - Then we make a link between __io_putchar and PUTCHAR_PROTOTYPE */
#if defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif

/* STEP 3 - when PUTCHAR_PROTOTYPE is called, we actually send ch character
 to our desired UART interface */
/**
 * @brief  Retargets the C library printf function to the USART.
 */
PUTCHAR_PROTOTYPE
{
   /* Transmit to UART3, the UART connected per default to STLink
   STLink will then forward all UART input to its virtual COM port and it can
   be read with any serial program like minicom on Linux */
   HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);
   return ch;
}
```

Then, as said, `STLink` will forward anything coming on its UART interface, to its virtual COM port: `/dev/ttyACM0` (or another name sometimes).

You can either monitor this output from minicom:
```bash
# Monitor serial output
minicom -D /dev/ttyACM0 -b 115200
```

Or, directly from VSCode when selecting menu `View`->`Serial Monitor`. You can then configure the serial output.

>[!HINT]
>Maybe it is both ways (you could send messages/commands to target) with this serial interface but I never tried it.

# Configure debugging chain on VSCode

>[!IMPORTANT]
>As I struggled to configure `launch.json` (see below) I only configured VSCode to work with STLink and `openOCD` as `gdb-server`. For other debugger, good luck with configuring `launch.json`.

I configured VSCode to interface itself with with `gdb-server`, using `gdb` and allowing me a more suitable GUI interface to debug.

I selected the plug-in **_Cortex-Debug_** from Marus25 because it is quite democratized and used amongst other VSCode users. 
- After installing it, a `.vscode/launch.json` file has to be configured
- Also, download the `STM32H723.svd` file (System View Description) from STMicroelectronics at the CAD page of STM32H7's official website:  [STM32H723/733 - STMicroelectronics](https://www.st.com/en/microcontrollers-microprocessors/stm32h723-733.html#cad-resources). It provides more info about memory, how registers are organized and so on.

Configuring the `launch.json` is the most horrible thing to do as there is thousands of parameters and poor documentation about those parameters is to be found.

After multiple tries and smoking all transistors of some AI, I got a working `launch.json`:
```json
   "version": "0.2.0",
   "configurations": [
      {
         "type": "cortex-debug",
         "name": "prj_h7_debug_openOcd",
         "cwd": "${workspaceRoot}/prj_h7/",
         "executable": "${workspaceFolder}/prj_h7/bin/prj_h7.elf",
         "request": "launch",
         "servertype": "openocd",
         "configFiles": [
            "${workspaceFolder}/prj_h7/stm32h7_debug.cfg"
         ],
         "searchDir": [],
         "runToEntryPoint": "main",
         "showDevDebugOutput": "none",
         "interface": "jtag", // swd also supported
         "svdFile": "${workspaceFolder}/prj_h7/STM32H723.svd",
         "device": "STM32H723ZGTx"
      }
   ]
```

>[!DANGER] Launch.json magic 
>How to configure this file is on my opinion poorly explained and I just it got working out of nowhere. No idea why. I took first some examples from the Internet, fought my ass off with this plugin's official documentation as explained before. Tried my best but no logic here... At the end I right clicked on the launch.json file, selected new Cortex-M debug session from the poped menu, adapted some path files and it worked.
>I mean, there is no explanation on how and which synthax to use for each parameter, what it exactly does and so on. It just ends in random tries. On I don't know how many parameters, good luck...

### In conclusion

I debug only on terminal, works more than fine and provide the 100% same amount of features and data as GUI version. It is just a little bit different. But this json magic got ma patience to an end. I was very deceived not to see what values each parameters may have, documentation is too poor.

# Run program

The program is quite easy and consists of three tasks:

`MsgSenderThreadOne_Entry()`, `MsgSenderThreadTwo_Entry()` and `MsgReceiverThread_Entry()`. It is based on the example of STM32H7 `ThreadX`.

It all worked fine, a deeper explanation about it won't be done here, as it would be too long and is not the purpose of this article.

#todo add picture of output.