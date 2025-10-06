

>[!INFO] Prerequisite: 
>You have  successfully compiled a `prj_h7.elf` from project `prj_h7/` (project for stm32h723zgtx) with **`-g3` debug flag** (maximal debug level = have symbols in your .elf file)

>[!HINT]
>How to set-up a debug session with `gdb` on Windows is very Linux-like. All steps will be explained here again.
>

**We will** 
1. first set all the different steps of debugging chain: prepare `gdb`, `gdb-server`, debugger's hardware...) prior to use it with VSCode.
2. Then we will configure VSCode to use this debugging environment. With a GUI it is more sexy.

## Start target and check

### Check presence of device

Plug your STM32H723ZG Nucleo or discovery board to your development laptop where you will run the debugging session. Be careful to have set this board (especially jumpers for STM's Nucleo and discovery boards) so that `STLink` and target are well connected and powered.

View of devices present on Windows:

#todo Add picture

Also, the virtual COM port should be available and detected in devices of Windows:

#todo Add picture

### `gdb-server` with `openOCD`

When all previous steps are good, just call the config file from prj_h7 to start `openOCD` as a `gdb-server`:

```bash
# Call openOCD with the config file
openocd -f <target_openocd>.cfg

# The following should be prompted:
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

Now it is ready to be used by `gdb` to start a debug session.

>[!NOTE]
>`gdb` may be called prior to `openOCD`, but if you try to connect to it with command 
>```
>target extended-remote :3333
>``` 
>It will not work, as no `gdb-server` is available. Call it again after starting `gdb-server` and all should be fine

### Final step: run `gdb` and start have fun

Leave the terminal with `openOCD` running, open a new one and go where your `prj_h7.elf` file is waiting to be debugged.

First step is to call `gdb` with your elf file as argument. Then in the `gdb` console (seen with `(gdb)` prompt), enter the command `target extended-remote :<port>` where `<port>` is the given port in `<debug_session_name>.cfg` file, line: `set GDB_PORT 3333`. It is also prompted when starting `gdb-server` (see previous step)

#### With `arm-none-eabi-gdb`

After `gdb-server` is running (either `openOCD` or `STLinkServer`) on another DOS-terminal, run `gdb` with the file you want to debug and attach to remote target through this freshly started `gdb-server`

```bash
# Start gdb
arm-none-eabi-gdb.exe prj_h7.elf

# The following should be prompted:
GNU gdb (Arm GNU Toolchain 14.3.Rel1 (Build arm-14.174)) 15.2.90.20241229-git
Copyright (C) 2024 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
...
Reading symbols from prj_h7.elf...

# Connect to gdb-server
(gdb) target extended-remote :3333
Remote debugging using :3333
Reset_Handler () at C:\Users\...\prj_blog\prj_h7\startup\startup_stm32h723zgtx.s:61
61        ldr   sp, =_estack      /* set stack pointer */

# Load (flash) prj_h7.elf on target:
(gdb) load
...

# Now you are ready to set breakpoints, run. stop, watch variables and so on
(gdb)
```

You are now ready to debug.

>[!WARNING]
>Be careful to cleanly quit any other gdb or gdb-server/openocd prior to start it with VSCode, some remanent instances may still work in background and VSCode may throw error like `openocd.exe ENOENT` (openocd not found) and you have to restart your full VSCode or do some actions to kill those stray threads/processes
>


>[!HINT]
>It is also possible to configure a startup script for `gdb` for it to automatically connect, flash and prepare target when it is called.
>You will find all explanations in [[Project 1 STM32H7 on Linux]]


## Look at `printf()` from project

Same as [[Project 1 STM32H7 on Linux]]

## Configure debugging chain on VSCode

Same as [[Project 1 STM32H7 on Linux]]


---
# Troubles with Windows and official STLinkServer

### `gdb-server` with official `STLinkServer` from STMicroelectronics

We will now use the official `STLinkServer` from STMicroelectronics to flash and debug on a Cortex-M target (the STM32H7)

I got the server from STM's official website: [ST-LINK-SERVER - ST-LINK server software module - STMicroelectronics](https://www.st.com/en/development-tools/st-link-server.html)

I downloaded the zip, unzipped it and started the .msi file. Install was like J.A.R.V.I.S. from Tony Stark installed its last powerful software on my computer, I couldn't grab my coffee that the server exec file was installed in:

```dos
C:\Program Files (x86)\STMicroelectronics\stlink_server
```


After starting the server and trying to connect with `arm-none-eabi-gdb`, I got error I couldn't solve:

From `gdb` side:
```dos
(gdb) target extended-remote 127.0.0.1:3333
Remote debugging using 127.0.0.1:3333
Ignoring packet error, continuing...
warning: unrecognized item "timeout" in "qSupported" response
```

From `gdb-server` (`STLinkServer`) side:
```dos
...
Stlk : 54 65691 : TCPCMD : unknown command received $vCont?#49
Debug: 55 65691 : process_stlink_tcp_cmd : returned value 0 unknown_command

Stlk : 56 67727 : TCPCMD : unknown command received $vCont?#49
Debug: 57 67734 : process_stlink_tcp_cmd : returned value 0 unknown_command
...
```


So I started to run with commands from STM32CubeIDE/.../plugins/ to find where `arm-none-eabi-gdb` are and also where the `STlink-Server` exec is (which is named `ST-LINK_gdbserver.exe`). I called them. Strangely, you have to call STM32CubeProgrammer with ST-LINK-Server for it to work:

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

So I did update the firmware of `STLink` debugger on Nucleo board with STMCubeProgrammer.

No better results with version 1.17.0 from STM32CubeIDE. After a firmware update of `STLink` debugger, results are not better. It still can't locate, even if I explicitly give the full path to it. 

Even after full path given, even the official `ST-Link-gdbserver` can't find its own `STLink` debugger:

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

So I gave up and stayed with `openOCD`.

# Run program

The program is quite easy and consists of three tasks:

`MsgSenderThreadOne_Entry()`, `MsgSenderThreadTwo_Entry()` and `MsgReceiverThread_Entry()`. It is based on the example of STM32H7 `ThreadX`.

It all worked fine, a deeper explanation about it won't be done here, as it would be too long and is not the purpose of this article.

#todo add picture of output.