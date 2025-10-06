
>[!Abstract]
>This section explains how to install one of the most versatile **`gdb-server`**: `openOCD`. It is used by almost all debuggers we will use in this tutorial. Segger (`JLink`) and STMicroelectronics (`STLink`) propose their own, but they are also compatible with `openOCD`

# Install `openOCD`

**Install:**
- Either with aptitude package manager on Linux:
```bash
sudo apt install openocd
```
- Or by cloning the git repo and put it where you want:
```bash
git clone https://github.com/openocd-org/openocd.git
```

### Some theory about `openOCD`

`openOCD` requires configuration files, which are specific for the debugger hardware, and for the targeted SoC (memory size, organization addresses, registers, etc...).

A huge quantity of target is supported by `openOCD`, from TI to STMicroelectronics, with some Qualcomm (and many other). After installing it, you may find them in `scripts/interface/` and `scripts/target/` folders which contains all important config files.

```bash
/usr/share/openocd $ ll
total 12
drwxr-xr-x  3 root root 4096 Nov 19  2024 contrib
drwxr-xr-x  2 root root 4096 Nov 19  2024 OpenULINK
drwxr-xr-x 11 root root 4096 Nov 19  2024 scripts
```

After I installed it, only the `openOCD` executable worked fine. Configuration scripts and other elements are rarely up to date or work. It is recommended by `openOCD` project to take microcontroller's vendor scripts and configurations:

>If you would rather be working "with" OpenOCD rather than "on" it, your operating system or JTAG interface supplier may provide binaries for you in a convenient-enough package. Such packages may be more stable than git mainline, where bleeding-edge development takes place.

\- OpenOCD's github repo

>[!IMPORTANT]
>After installing openOCD, it is recommended to manually update/modify its script files with the one from targe's vendor. As we will do for STM32 targets


# Use `openOCD`

### `openOCD` hack, how to take vendor's scripts and configurations

So for us to have it working with STLink-V3E from STMicroelectronics, we will cheat a little bit. We will use the one provided by STM32CubeIDE and put them in our `openOCD` folder.

>[!INFO] 
>How is it done explained here, is for an STM32H723ZG target, with integrated STLink. But it should be the same for other targets, even with external STLink.



>[!TIP] Cheating with STM32CubeIDE
>Actually, when debugging with OpenOCD on STMCubeIDE, it does nothing less than call its own, bundled OpenOCD which is hidden in: `C:\...\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.debug.openocd_2.3.100.202501240831\resources\openocd\st_scripts\`
>Doing the same as STMCudeIDE, without STMCubeIDE is actually simple

>[!WARNING] OpenOCD is adapted for STM32CubeIDE
>Actually, when debugging with OpenOCD on STMCubeIDE, it does nothing less than call its own, bundled OpenOCD. Indeed, STM32CubeIDE integrates its own OpenOCD executable and configuration which can be found here:
>`C:\...\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.debug.openocd_xxx\resources\openocd\st_scripts\`
>`C:\...\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.openocd.win32_xxx\tools\`
>
>This implementation is more up to date than the scripts provided by OpenOCD git repo and way much than the last release of OpenOCD in aptitude packages.
>
>Doing the same as STMCudeIDE, without STMCubeIDE is actually simple.
>Which we will do.


### Cheat on OpenOCD to use STM's OpenOCD scripts.

Here, it is up to the user, how to set path and where to put those STM's script and config file. What I did is add them into` /usr/share/openocd/` but with an `st_` prefix so that I can tell `openOCD` to specifically fetch those up-to-date, vendor's certified config and script files:

Before modification of `openOCD`'s folder:

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
>I had to backup old `mem_helper.tcl` file to `old_mem_helper.tcl` file because this name is used by all scripts and config files. Same for `gdb_helper.tcl`. At beginning I wanted to add `st_` prefix, but modifying all other config file would have been a mess...
>
>```bash
># Backup original files
>sudo mv mem_helper.tcl old_mem_helper.tcl
>
># After modifs
>ll
>-rw-r--r-- 1 root    root     1021 Feb 16  2023 mem_helper.tcl
-rw-r--r-- 1 root    root     4419 Feb 16  2023 memory.tcl
-rw-r--r-- 1 root    root     2121 Feb 16  2023 mmr_helpers.tcl
drwxr-xr-x 2 root    root     4096 Jul 11 13:19 st_board
-rw-r--r-- 1 jeremie jeremie   510 Apr  9 23:34 st_gdb_helper.tcl
drwxr-xr-x 2 root    root     4096 Jul 11 13:24 st_interface
-rw-r--r-- 1 jeremie jeremie   976 Apr  9 23:34 st_mem_helper.tcl
>```

Another solution would be to put STM32CubeIDE's `openOCD` script in `/home/` folder, somewhere and instead of giving a relative path, like `source [find st_interface/stlink-dap.cfg]` argument in `debug_session.cfg` file (cfg file explained below), put an absolute path to where you put the STM32CubeIDE's `openOCD` script files. Like: `source [find /home/path/to/your/scripts/stlink-dap.cfg]`

**Schematic of `openOCD` hacking**:
![[OpenOCD_hack.svg]]

## Configure for target:

**Configure:**
`openOCD` can be started with a `tcl` configuration `<debug_session_name>.cfg` file. It contains some important, and user specific descriptions:

```tcl
# Example of an openOCD config for an stm32h723zgtx target

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

**How to run it**:
```bash
openocd -f <debug_session_name>.cfg
```

**Modifications of `config.tcl` file**
This config file may be extended, modified as pleased by user. I took one config file generated by STMCubeIDE. `interface/stlink-dap.cfg` and `target/stm32h7x.cfg` from STMicroelectronics are however important and should not be altered.


>[!INFO] Working on other targets with other debuggers:
>You will also find the other `target/` and `interface/` script files required if you work with other traget and/or debugger.
>For example, if working on an STM32F4, with JLink hardware, change `source` lines in .cfg script for:
>`interface/jlink.cfg`
>`target/stm32f4x.cfg`

I placed the `<debug_session_name>.cfg` in my project root folder because it is specific to my project, but it can placed/organized with other config file as you want. No rule here, just be sure to call the good one.
