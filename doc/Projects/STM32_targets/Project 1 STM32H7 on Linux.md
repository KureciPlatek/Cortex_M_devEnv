
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

### `gdb-server` with `openOCD`

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

### `gdb-server` with `st-util` from stlink-org

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


## Look at `printf()` from project

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

## Configure debugging chain on VSCode

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