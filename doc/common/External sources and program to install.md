# On Linux
## Install and configure OpenOCD

**Install:**
- Either with aptitude package manager on Linux:
```bash
sudo apt install openocd
```
- Or by cloning the git repo and put it where you want:
```bash
git clone https://github.com/openocd-org/openocd.git
```

I did first option, only the openocd executable works fine, config scripts and other are rarely up to date or work. It is recommended by OpenOCD project to take microcontroller's vendor scripts and config

>If you would rather be working "with" OpenOCD rather than "on" it, your operating system or JTAG interface supplier may provide binaries for you in a convenient-enough package. Such packages may be more stable than git mainline, where bleeding-edge development takes place.

\- OpenOCD's github repo

### OpenOCD hack

OpenOCD requires configuration files, which are specific for the debugger hardware, and for the targeted SoC. This way, it knows hot memory is organized, what are the difference specifications and so on.

Almost all supported OpenOCD SoC have their config file in OpenOCD install folder, but those may be old and doesn't work! 

So for us to have it working with STLink-V3E from STMicroelectronics, we will cheat a little bit. We will use the one provided by STM32CubeIDE and put them in our OpenOCD folder.

>[!INFO] 
>How is it done explained here, is for an STM32H723ZG target, with integrated STLink

>[!TIP] Cheating with STM32CubeIDE
>Actually, when debugging with OpenOCD on STMCubeIDE, it does nothing less than call its own, bundled OpenOCD which is hidden in: `C:\...\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.debug.openocd_2.3.100.202501240831\resources\openocd\st_scripts\`
>Doing the same as STMCudeIDE, without STMCubeIDE is actually simple

>[!WARNING] OpenOCD is adapted for STM32CubeIDE
>STM32CubeIDE actually integrates its own OpenOCD executable and configuration. It can be found here:
>`C:\...\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.debug.openocd_xxx\resources\openocd\st_scripts\`
>`C:\...\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.openocd.win32_xxx\tools\`
>
>This implementation is more up to date than the scripts provided by OpenOCD git repo and way much than the last release of OpenOCD in aptitude packages.
>OpenOCD project strongly recommends to use the implementation of OpenOCD made by the chip provider.
>Which we will do.

When installing OpenOCD through aptitude package manager, you will obtain a list of config files, they are available inside the installed OpenOCD repository on Linux:

```bash
/usr/share/openocd $ ll
total 12
drwxr-xr-x  3 root root 4096 Nov 19  2024 contrib
drwxr-xr-x  2 root root 4096 Nov 19  2024 OpenULINK
drwxr-xr-x 11 root root 4096 Nov 19  2024 scripts
```
In `scripts/`, you will find `interface/` and `target/` folders which contains all important config files.

>[!WARNING] THOSE CONFIG FILE MAY NOT WORK!!!
>As said before

Use the ones provided by STM32CubeIDE.

#### Cheat on OpenOCD to use STM's OpenOCD scripts.

Here, it is up to the user, how to set path and where to put those STM's script and config file. What I did is add them into /usr/share/openocd/ but with an st_ prefix so that I can tell OpenOCD to specifically fetch those up-to-date, vendor's certified config and script files:

Before modification of OpenOCD's folder:

```bash
/usr/share/openocd/scripts $ ll
total 84
-rw-r--r-- 1 root root  1659 Feb 16  2023 bitsbytes.tcl
drwxr-xr-x 3 root root 20480 Nov 19  2024 board
drwxr-xr-x 5 root root  4096 Nov 19  2024 chip
drwxr-xr-x 2 root root  4096 Nov 19  2024 cpld
drwxr-xr-x 4 root root  4096 Nov 19  2024 cpu
drwxr-xr-x 2 root root  4096 Nov 19  2024 fpga
drwxr-xr-x 4 root root  4096 Jul 11 12:40 interface
-rw-r--r-- 1 root root  1021 Feb 16  2023 mem_helper.tcl
-rw-r--r-- 1 root root  4419 Feb 16  2023 memory.tcl
-rw-r--r-- 1 root root  2121 Feb 16  2023 mmr_helpers.tcl
drwxr-xr-x 4 root root 16384 Jul 11 12:43 target
drwxr-xr-x 2 root root  4096 Nov 19  2024 test
drwxr-xr-x 2 root root  4096 Nov 19  2024 tools
```

After modifications:
```bash
/usr/share/openocd/scripts $ ll
total 104
-rw-r--r-- 1 root    root     1659 Feb 16  2023 bitsbytes.tcl
drwxr-xr-x 3 root    root    20480 Nov 19  2024 board
drwxr-xr-x 5 root    root     4096 Nov 19  2024 chip
drwxr-xr-x 2 root    root     4096 Nov 19  2024 cpld
drwxr-xr-x 4 root    root     4096 Nov 19  2024 cpu
drwxr-xr-x 2 root    root     4096 Nov 19  2024 fpga
-rw-r--r-- 1 jeremie jeremie   510 Apr  9 23:34 gdb_helper.tcl
drwxr-xr-x 4 root    root     4096 Jul 11 12:40 interface
-rw-r--r-- 1 jeremie jeremie   976 Apr  9 23:34 mem_helper.tcl
-rw-r--r-- 1 root    root     4419 Feb 16  2023 memory.tcl
-rw-r--r-- 1 root    root     2121 Feb 16  2023 mmr_helpers.tcl
-rw-r--r-- 1 root    root     1021 Feb 16  2023 old_mem_helper.tcl
drwxr-xr-x 2 root    root     4096 Jul 11 13:19 st_board
drwxr-xr-x 2 root    root     4096 Jul 11 13:24 st_interface
drwxr-xr-x 2 root    root     4096 Jul 11 13:20 st_target
drwxr-xr-x 4 root    root    16384 Jul 11 12:43 target
drwxr-xr-x 2 root    root     4096 Nov 19  2024 test
drwxr-xr-x 2 root    root     4096 Nov 19  2024 tools
```

With `st_*/` folders containing all scripts provided by STM32CubeIDE.

>[!WARNING] 
>I had to modify old `mem_helper.tcl` file to be `old_mem_helper.tcl` file because this name is used by all scripts and config file. Same for `gdb_helper.tcl`. At beginning I wanted to add st_ prefix, but modifying all other config file would have been a mess...
>Otherwise, openocd/ folder would have been more clean:
>```bash
>-rw-r--r-- 1 root    root     1021 Feb 16  2023 mem_helper.tcl
-rw-r--r-- 1 root    root     4419 Feb 16  2023 memory.tcl
-rw-r--r-- 1 root    root     2121 Feb 16  2023 mmr_helpers.tcl
drwxr-xr-x 2 root    root     4096 Jul 11 13:19 st_board
-rw-r--r-- 1 jeremie jeremie   510 Apr  9 23:34 st_gdb_helper.tcl
drwxr-xr-x 2 root    root     4096 Jul 11 13:24 st_interface
-rw-r--r-- 1 jeremie jeremie   976 Apr  9 23:34 st_mem_helper.tcl
>```

Another solution would be to put STM32CubeIDE's OpenOCD script in /home/ folder, somewhere and instead of giving a relative path, like `source [find st_interface/stlink-dap.cfg]` argument in `debug_session.cfg` file (cfg file explained below), put an absolute path to where you put the STM32CubeIDE's OpenOCD script files. Like: `source [find /home/path/to/your/scripts/stlink-dap.cfg]`

#### After OpenOCD's hack:

**Configure:**
`OpenOCD` has to be configured with a `<debug_session_name>.cfg` file. It contains some important, and user specific descriptions:

```
# This is an genericBoard board with a single STM32H723ZGTx chip
# Generated by STM32CubeIDE

# Debugger's info
source [find interface/stlink-dap.cfg]
# CPU variables
source [find target/stm32h7x.cfg]

set WORKAREASIZE 0x8000
transport select "dapdirect_swd"
set CHIPNAME STM32H723ZGTx
set BOARDNAME genericBoard

# Enable debug when in low power modes
set ENABLE_LOW_POWER 1

# Stop Watchdog counters when halt
set STOP_WATCHDOG 1

# STlink Debug clock frequency
set CLOCK_FREQ 8000

# Reset configuration
reset_config srst_only srst_nogate connect_assert_srst
set CONNECT_UNDER_RESET 1
set CORE_RESET 0

# ACCESS PORT NUMBER
set AP_NUM 0

# GDB PORT
set GDB_PORT 3333
```

This config file may be extended, modified as pleased by user. I took one config file generated by STMCubeIDE. `interface/stlink-dap.cfg` and `target/stm32h7x.cfg` are however important. 

>[!INFO] Working on other targets with other debuggers:
>You will also find the other `target/` and `interface/` script files required if you work with other traget and/or debugger.
>For example, if working on an STM32F4, with JLink hardware, change `source` lines in .cfg script for:
>`interface/jlink.cfg`
>`target/stm32f4x.cfg`

I placed the `<debug_session_name>.cfg` in my project root folder because it is specific to my project, but it can placed/organized with other config file as you want. No rule here, just be sure to call the good one.

## Install GNU-ARM toolchain

**On Linux, either with aptitude:**
```bash
sudo apt install gcc-arm-none-eabi
```

>[!WARNING]
>I discovered that installing GNU ARM toolchain per package manager, doesn't provide the gnu debugger with it (`arm-none-eabi-gdb`, which is the equivalent of `gdb`, but for ARM based microcontrollers). `gdb-multiarch` is then required and works fine

**Or download the last version from official's website directly: **
[Arm GNU Toolchain Downloads – Arm Developer](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

Unpack it and place it in `/usr/share/`, then add links in `/usr/bin`:
```bash
$ sudo tar xJf arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi.tar.xz -C /usr/share/
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-g++ /usr/bin/arm-none-eabi-g++
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-size /usr/bin/arm-none-eabi-size
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-objcopy /usr/bin/arm-none-eabi-objcopy
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-ld /usr/bin/arm-none-eabi-ld
```
The list of linked executables here is reduced to some, but if your development environment requires more, just add them. You will see that in `/usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/` many more tools are available. Like `objdump` or `strip`.

It depends on what you require for your project (compiling, stripping, dumping of object etc...).

This way, you will have an apt-like install of most recent and official gnu-arm tool-chain release. But it keeps the manually installed file separated from Linux's apt installed files.

## Run it all together

>[!INFO] Prerequisite: 
>You have  successfully compiled a _program.elf_ with -g3 debug flag while compiling (put maximal debug level = have symbols in your .elf file)

We will first set all the different steps of debugging chain (gdb, gdb-server, debugger's hardware...) prior to use it with VSCode. Then we will configure VSCode tu use this debugging environment. With a GUI it is more sexy.

Plug your Nucleo or discovery board to your development laptop where you will run the debugging session. Be careful to have set this board (especially jumpers for STM's nucleo and discovery boards) so that STLink and target are well connected and powered.

On my case, I have an STM32H723 Nucleo board and after plugging it, I see the following devices appears on Linux:
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

### gdb-server with OpenOCD

When all previous steps are good, just call this config file to start OpenOCD as a gdb-server:
```bash
~/.../prj_h7 $ openocd -f stm32h7_debug.cfg
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

Now it is ready to be used by gdb to start a debug session.

### gdb-server with st-util from stlink-org

Also, if previous install and config steps are good, you could run `st-util` and see on terminal:

```bash
$ st-util
st-util
2025-07-11T11:14:50 INFO usb.c: Unable to match requested speed 1800 kHz, using 1000 kHz
2025-07-11T11:14:50 INFO common.c: H72x/H73x: 128 KiB SRAM, 1024 KiB flash in at least 128 KiB pages.
2025-07-11T11:14:50 INFO gdb-server.c: Listening at *:4242..
```

Which tells us that gdb-server from the open-source STLink server recognized the STM32H723 as target and is ready to start a debug session

### Final step: run gdb and start have fun

Leave the terminal with openocd running, open a new one and go where your `program.elf` file is waiting to be debugged.

First step is to call gdb with your elf file as argument. Then in the gdb console (seen with `(gdb)` prompt), enter the command `target extended-remote :<port>` where `<port>` is the given port in `<debug_session_name>.cfg` file, line `set GDB_PORT 3333`.

**With `arm-none-eabi-gdb`**

```bash
~/.../prj_h7/bin $ arm-none-eabi-gdb <project_name>.elf
GNU gdb (Arm GNU Toolchain 14.3.Rel1 (Build arm-14.174)) 15.2.90.20241229-git
Copyright (C) 2024 Free Software Foundation, Inc.
...
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from prj_h7.elf...
(gdb) target extended-remote :3333
Remote debugging using :3333
Reset_Handler () at /home/jeremie/work/prj_blog/prj_h7/startup/startup_stm32h723zgtx.s:61
61        ldr   sp, =_estack      /* set stack pointer */
(gdb)
```

Now you are ready to load, run, stop, breakpoint and do everything you need to debug your program.

**With `gdb-multiarch`**

```bash
~/.../prj_h7/bin $ gdb-multiarch prj_h7.elf
GNU gdb (Debian 13.1-3) 13.1
Copyright (C) 2023 Free Software Foundation, Inc.
...
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from prj_h7.elf...
(gdb) target extended-remote :3333
Remote debugging using :3333
Reset_Handler () at /home/jeremie/work/prj_blog/prj_h7/startup/startup_stm32h723zgtx.s:61
61        ldr   sp, =_estack      /* set stack pointer */
(gdb)
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

## Configure debugging chain on VSCode

I configured VSCode to interface itself with with gdb-server, using gdb and allowing me a more suitable GUI interface to debug.
Pluging Cortex-Debug from Marus25 is quite spread and used it seems. For that, `.vscode/launch.json` file has to be configured, it is the most horrible thing to do as there is thousands of parameters and poor documentation about those parameters is to be found.

Install **_Cortex-Debug_** extension from Marus and download the .svd file (System View Description) from STMicroelectronics at the CAD page of STM32H7's official website:  [STM32H723/733 - STMicroelectronics](https://www.st.com/en/microcontrollers-microprocessors/stm32h723-733.html#cad-resources) It provides more info about memory, how registers are organized and so on

Snippet of my `launch.json` that worked:

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

I debug only on terminal, works more than fine and provide the 100% same amount of features and data as GUI version.
## Install of Pico-SDK

todo


# On Windows

I installed the [Arm GNU Toolchain Downloads – Arm Developer](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) from ARM official website. Windows MinGW x86_64 hosted Windows. I have no idea if there is a remanent MinGW or Cygwin somewhere on my computer, so maybe some conflict may arise.

Toolchain is placed here:
```bash
C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.3 rel1\bin>
```
#### Beware of PATH

>[!WARNING] Path conflict?
>I got the following error:
>```dos
>[100%] Linking C executable prj_h7.elf
c:/progra~2/gnuarm~1/102021~1.10/bin/../lib/gcc/arm-none-eabi/10.3.1/../../../../arm-none-eabi/bin/ld.exe:C:/Users/jeremie.gallee/Documents/Project_MarbleFrame/prj_blog/prj_h7/link/STM32H723ZGTX_FLASH.ld:91: non constant or forward reference address expression for section .ARM.extab
collect2.exe: error: ld returned 1 exit status
mingw32-make[2]: *** [CMakeFiles\prj_h7.elf.dir\build.make:3621: prj_h7.elf] Error 1
mingw32-make[1]: *** [CMakeFiles\Makefile2:82: CMakeFiles/prj_h7.elf.dir/all] Error 2
mingw32-make: *** [Makefile:90: all] Error 2
> ```
> Which may come from search for an old ARM-GCC, conflicting on my Windows.
> Indeed, this error comes from linker.ld file in my project, which specifies that GCC10 is not supported, only from GCC11:
> ```
> .ARM.extab (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
> {
>    *(.ARM.extab* .gnu.linkonce.armextab.*)
>    ```
> From linker file

I had indeed in my PATH, STMCube, which was placed before, and also an old ARM-GNU compiler:

```dos
C:\Windows\System32\OpenSSH\;
C:\msys64\ucrt64\bin;
C:\ST\STM32CubeIDE_1.14.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.make.win32_2.1.101.202401061624\tools\bin;
C:\Program Files (x86)\GNU Arm Embedded Toolchain\10 2021.10;
C:\Program Files\usbipd-win\;
C:\Program Files\TortoiseSVN\bin;
```
The problem was that I got an error when installing the last ARM-GNU toolchain, path was too long for Windows, which said to me to manage it by myself the operation of adding the newly installed gnu-arm, version 14 to PATH variable. But 1000 MB for Edge when opening two pdf is ok...

So after correction to last installed ARM-GNU, my path is now:

```dos
C:\msys64\ucrt64\bin;
C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.3 rel1\bin;
C:\ST\STM32CubeIDE_1.14.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.make.win32_2.1.101.202401061624\tools\bin;
C:\Program Files\usbipd-win\;
C:\Program Files\TortoiseSVN\bin;
```

Which is better. Now it all works to compile on Windows.

### STM32CubeMX and STM32CubeIDE

Those two big babies are required to generate a couple of files, which won't be heavier at the end, as a couple of kbyte (surely less).

They generate, complex startup and linker file and are quite handy to generate clock, pll and system configuration. It could also be possible to do those parts which are in C (not linker and startup.s which I recommend take some already made) ourselves, but it would take too much time.


>[!WARNING] 
>Every time you create a new project with STM32CubeMX and/or STM32CubeIDE, it will actually also do a git clone of the STM32CubeXX folders we manually cloned!!! Again, bundle all together again. It is, yeah, explained when generating a project with Cube. 
>But now I have a file in my `%HOME%` older which is more than 11 Gbyte!!:
>On `~\STM32Cube\Repository`, size is:
>```bash
>C:\Users\jeremie.gallee\STM32Cube
>11.6 GB (12’458’266’786 Bytes)
>```
>PS: Getting this info on DOS is a farce, you have to do `dir /s` and wait for a couple of ~hours~ minutes for terminal to end print it all and show value...

>[!WARNING]
>After a little bit of flowing into this STM32Cube/Repository mystic folder I discovered some pepitos, truly.
>STM32CubeMX created 3 different STM32F4 HAL folders:
>```dos
>~\STM32Cube\Repository\Packs\STMicroelectronics\X-CUBE-AZRTOS-F4\1.1.0\Drivers\STM32F4xx_HAL_Driver
>~\STM32Cube\Repository\STM32Cube_FW_F4_V1.28.1\Drivers\STM32F4xx_HAL_Driver
>~\STM32Cube\Repository\STM32Cube_FW_F4_V1.28.2\Drivers\STM32F4xx_HAL_Driver
>```
>I mean, whaaaaat??? If they are different, gosh...


**Total of space used by all STMCube programs:**

@todo

## Configure debug environment

### With OpenOCD ad gdb-server

How to set-up OpenOCD on Windows is very Linux-like. You have to install the exectuable, the most official way, but you have to cheat on config files path. All steps will be explained here again.

**To install it**, I took the official one from OpenOCD.org website, which is on github repository:
[Releases · xpack-dev-tools/openocd-xpack](https://github.com/xpack-dev-tools/openocd-xpack/releases)
I took the win32: [xpack-openocd-0.12.0-6-win32-x64.zip](https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.12.0-6/xpack-openocd-0.12.0-6-win32-x64.zip), unpacked it and moved it to Program Files x86.

After my fan died over CPU's overheating because I copied 2.7 MB into `C:\Program Files (x86)\` (Cisco was doing some bitcoin mining I guess...) I canceled the operation and moved the exe somewhere else in my HOME environment and resigned myself to add this strange path to my `%PATH%` variable. I hope Windows will support it when it is too long.

Anyway I made it to copy openocd to `C\Program Files (x86)\xpack-openocd<version>`.  After that, I had to add openocd to `%PATH%` variable, as well as path to the scripts. My `%PATH%` variable now look like:

```dos
$> echo %PATH%
...
C:\Program Files (x86)\xpack-openocd-0.12.0-6\bin;
C:\Program Files (x86)\xpack-openocd-0.12.0-6\openocd\scripts;
...
```

The only thing left was to copy again, all openocd scripts provided by STMCubeIDE, like I did for Linux. I have then on my laptop the following OpenOCD environment:

```dos
C:\Program Files (x86)\xpack-openocd-0.12.0-6\openocd\scripts\dir

21.07.2025  14:27    <DIR>          .
21.07.2025  14:27    <DIR>          ..
07.02.2025  12:09             1’659 bitsbytes.tcl
07.02.2025  12:09    <DIR>          board
07.02.2025  12:09    <DIR>          chip
07.02.2025  12:09    <DIR>          cpld
07.02.2025  12:09    <DIR>          cpu
07.02.2025  12:09    <DIR>          fpga
09.04.2025  23:34               510 gdb_helper.tcl
07.02.2025  12:09    <DIR>          interface
07.02.2025  12:09             4’423 memory.tcl
09.04.2025  23:34               976 mem_helper.tcl
07.02.2025  12:09             2’121 mmr_helpers.tcl
07.02.2025  12:09             1’021 old_mem_helper.tcl
21.07.2025  14:27    <DIR>          st_interface
21.07.2025  14:27    <DIR>          st_target
07.02.2025  12:09    <DIR>          target
07.02.2025  12:09    <DIR>          test
07.02.2025  12:09    <DIR>          tools
               6 Datei(en),         10’710 Bytes
              13 Verzeichnis(se), 219’145’367’552 Bytes frei
```

Where `st_` folders are to be found.

After that, just start again, like for Linux, either per command line, by executing in one DOS-terminal:
```dos
$> cd path\to\target_openocd\config\file
$> openocd -f <target_openocd>.cfg
xPack Open On-Chip Debugger 0.12.0+dev-01850-geb6f2745b-dirty (2025-02-07-10:08)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
stm32h7x_cti_prepare_restart_one
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : STLINK V3J6M2 (API v3) VID:PID 0483:374E
...
```
Then on another DOS-terminal, run gdb with the file you want to debug and attach to remote target through this freshly started gdb-server

```dos
>arm-none-eabi-gdb.exe prj_h7.elf
GNU gdb (Arm GNU Toolchain 14.3.Rel1 (Build arm-14.174)) 15.2.90.20241229-git
Copyright (C) 2024 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
...
Reading symbols from prj_h7.elf...
(gdb) target extended-remote :3333
Remote debugging using :3333
Reset_Handler () at C:\Users\...\prj_blog\prj_h7\startup\startup_stm32h723zgtx.s:61
61        ldr   sp, =_estack      /* set stack pointer */
(gdb)
```

You are now ready to debug

>[!WARNING]
>Be careful to cleanly quit any other gdb or gdb-server/openocd prior to start it with VSCode, some remanent instances may still work in background and VSCode may throw error like `openocd.exe ENOENT` (openocd not found) and you have to restart your full VSCode or do some actions to kill those stray threads/processes
>

### With official STLinkServer as gdb-server

We will now use the official STLinkServer from STMicroelectronics to flash and debug on a Cortex-M target (the STM32H7)

I got the server from STM's official website: [ST-LINK-SERVER - ST-LINK server software module - STMicroelectronics](https://www.st.com/en/development-tools/st-link-server.html)
I downloaded the zip, unzipped it and started the .msi file. Install was like J.A.R.V.I.S. from Tony Stark installed its last powerful software on my computer, I couldn't grab my coffee that the server exec file was installed in:

```dos
C:\Program Files (x86)\STMicroelectronics\stlink_server
```

After starting the server and trying to connect with arm-none-eabi-gdb, I got error I couldn't solve. 
From gdb side:
```dos
(gdb) target extended-remote 127.0.0.1:3333
Remote debugging using 127.0.0.1:3333
Ignoring packet error, continuing...
warning: unrecognized item "timeout" in "qSupported" response
```
From gdb-server side:
```dos
...
Stlk : 54 65691 : TCPCMD : unknown command received $vCont?#49
Debug: 55 65691 : process_stlink_tcp_cmd : returned value 0 unknown_command

Stlk : 56 67727 : TCPCMD : unknown command received $vCont?#49
Debug: 57 67734 : process_stlink_tcp_cmd : returned value 0 unknown_command
...
```

So I started to run with commands from STM32CubeIDE/.../plugins/ to find where arm-none-eabi-gdb are and also where the STlink-Server exec is (which is named `ST-LINK_gdbserver.exe`). I called them. Strangely, you have to call STM32CubeProgrammer with ST-LINK-Server for it to work:

Path to STLink-server:
```dos
C:\ST\STM32CubeIDE_1.18.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.stlink-gdb-server.win32_2.2.100.202501151542\tools\bin
```
Call of STLink-server:
```dos
ST-LINK_gdbserver.exe -e -d -cp  C:\ST\STM32CubeIDE_1.18.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.2.100.202412061334/tools/bin
```

But the following output happened:

```dos
STMicroelectronics ST-LINK GDB server. Version 7.10.0
Copyright (c) 2025, STMicroelectronics. All rights reserved.

Starting server with the following options:
        Persistent Mode            : Enabled
        Logging Level              : 1
        Listen Port Number         : 61234
        Status Refresh Delay       : 15s
        Verbose Mode               : Disabled
        SWD Debug                  : Enabled


Error in initializing ST-LINK device.
Reason: ST-LINK firmware upgrade required. Please upgrade the ST-LINK firmware using the upgrade tool.
```

And I am not sure if the new ST-LINK (software programmed on the ST-Link debugger, on the Nucleo board) is compatible with the OpenOCD config I could make work. So not sure if I want to do it.

No better results with version 1.17.0 from STM32CubeIDE. After a firmware update of STLink debugger, results are not better. It still can't locate STMCubeProgrammer (`STM32_Programmer_CLI.exe`), even if I explicitly give the full path to it. 

Even after full path given, even the official ST-Link-gdbserver can't find its own STLink debugger:

```dos
C:\ST\STM32CubeIDE_1.18.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.stlink-gdb-server.win32_2.2.100.202501151542\tools\bin>ST-LINK_gdbserver.exe -cp C:\ST\STM32CubeIDE_1.18.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.2.100.202412061334\tools\bin


STMicroelectronics ST-LINK GDB server. Version 7.10.0
Copyright (c) 2025, STMicroelectronics. All rights reserved.

Starting server with the following options:
        Persistent Mode            : Disabled
        Logging Level              : 1
        Listen Port Number         : 61234
        Status Refresh Delay       : 15s
        Verbose Mode               : Disabled

Target no device found

Error in initializing ST-LINK device.
Reason: No device found on target.
```

So I gave up

>[!CONCLUSION]
>OpenOCD works fine and is kept quite decoupled from the monolithic STM32CubeIDE.
>Even work with st-util open source can better discover the STLink and target.
>So I will keep on working with open-source devices and tools

### General troubleshoot with Windows environment

>[!TIP] Stop thinking Windows is good
>But we should stop to work on Windows OMO, it just tries to reach the capacity of development of Linux, and so many things are just there to try to imitate Linux, like Cygwin or MinGW, then Microsoft itself did the WSL, which adds more complexity (even if WSL is great) to the stuff.
>
>Windows is just evolving backwards, like the new _"AI powered"_ Outlook is, this thing is the biggest pile of junk I ever saw, 1/2 a Gb for sometimes blocking my e-mail because it thinks I am focusing! Please just stop putting intelligence everywhere, I don't want my hammer to get soft before hitting the nail, because it thinks I may hurt myself...)
>
>The only things that are keeping Windows afloat is because of Outlook (which you know my opinion on), Excel and Office suite, and also only because everybody base it works on it. And also because some program still only work on Windows.
>
>WSL is on my opinion also not much better, because it is not a full kernel and for example, some drivers for PEAK-PCAN may not work (have ever heard of error "HEADER not found"?). Also, at some point virtual memory manager Vmmem will take over 1.5 Gb of RAM space...
>
>Also with WSL, you have to do some trick to forward USB ports to this semi-OS, so that you can flash and debug your program on target with gdb (I had to use usbipd to forward STLinkDebugger to WSL env)

Windows only brings problems. It is always a strange state regarding terminals, it is never clear how it will react, as sometimes it configures itself to be UNIX like, sometimes half, sometimes not. Git reacts differently.

Those backslashes are nothing else but a pain in the ass

It is still crazy to have an space in name `Program Files`.

And those CRLF in files are just horrible. I mean, why?? I don't know how many times, with the flourishing of "smart" programs and IDE, it configures it back to CRLF, I didn't went to control it at each commit (please don't tell me I have to do that) and tadaaaa `^M` chars everywhere! Big Red Commit!