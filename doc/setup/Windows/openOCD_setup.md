

>[!Abstract]
>This section explains how to install one of the most versatile **`gdb-server`**: `openOCD`. It is used by almost all debuggers we will use in this tutorial. Segger (`JLink`) and STMicroelectronics (`STLink`) propose their own, but they are also compatible with `openOCD`

>[!HINT]
>How to set-up `openOCD` on Windows is very Linux-like. You have to install the executable the most official way, but you have to cheat on config files path. All steps will be explained here again.

# Install `openOCD`

**Install:**
I took the official one from OpenOCD.org website, which is on github repository:
[Releases · xpack-dev-tools/openocd-xpack](https://github.com/xpack-dev-tools/openocd-xpack/releases)
I took the win32: [xpack-openocd-0.12.0-6-win32-x64.zip](https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.12.0-6/xpack-openocd-0.12.0-6-win32-x64.zip), unpacked it and moved it to Program Files x86.


After my fan died over CPU's overheating because I copied 2.7 MB into `C:\Program Files (x86)\` (Cisco was doing some bitcoin mining I guess...) I canceled the operation and moved the exe somewhere else in my `%HOME%` environment and resigned myself to add this strange path to my `%PATH%` variable. I hope Windows will support it when it is too long.

Anyway I made it to copy `openOCD` to `C\Program Files (x86)\xpack-openocd<version>`.  After that, I had to add `openOCD` to `%PATH%` variable, as well as path to the scripts. My `%PATH%` variable now look like:

```dos
$> echo %PATH%
...
C:\Program Files (x86)\xpack-openocd-0.12.0-6\bin;
C:\Program Files (x86)\xpack-openocd-0.12.0-6\openocd\scripts;
...
```

The only thing left was to copy again, all `openOCD` scripts provided by STMCubeIDE, like I did for Linux. I have then on my laptop the following `openOCD` environment:

See more details here: [how to cheat on openOCD](../Linux/openOCD_setup#Cheat on OpenOCD to use STM's OpenOCD scripts.)

Here a print of my `openocd/`'s folder after modifications
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

## Configure for target:

Exactly the same as for Linux description in: [configure for target](../Linux/openOCD_setup#Configure for target:)

**How to run it**:
Again, the same as for Linux

```shell
# Go where your project is
cd path\to\target_openocd\config\file

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

