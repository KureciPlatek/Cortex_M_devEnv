
**gdb** is only the debugger, it knows only to stop, run, watch, breakpoint and so on, on C/C++/ASM (and others) languages. How uC hardware is made, how to set breakpoint is for it unknown.

That's when **gdb-server** comes in. It does this stuff, and gdb will connect to this server, to start a debug session (actually also flashing, erasing and other usesful features when working on a uC)

This gdb-server has to know what kind of architecture we are dealing with, used memory technology, clock frequency of debugger (STLINK, JLink...), configure its server and so on. Sometimes it is automatic, sometimes you have to configure yourself (it is the case of OpenOCD for example)

Then, last part is to connect your **debugger's hardware** to the board, through JTAG, SWD or other interface/protocol, and start the gdb-server of your choice.

>[!TIP]
>As we now know that gdb-server and debugger hardware are two different entities, it should be clear that some gdb-servers supports some hardware, some don't and some gdb-server are more suited for a hardware than others. 
>
>For example STLINK-V3E works better with the official `STLinkServer` than the open-source project `stlink-org` ([GitHub - stlink-org/stlink: Open source STM32 MCU programming toolset](https://github.com/stlink-org/stlink)) 

From Marus's blog:

> You may know them as OpenOCD, pyOCD, JLinkGDBServer, STLinkServer, etc. GDB by itself does not know anything about your device. It does know what an ARM architecture looks like but has no clue about much else. It relies on a program called a gdb-server that provides that knowledge and services. Among services, the **gdb-server helps create breakpoints**. GDB knows the instruction address of the breakpoint but does not know how to create a breakpoint in the device (especially for a FLASH-based device, which registers to set, etc.) **GDB does NOT know how to load** (program) the device with the executable file. **It asks the gdb-server to load** the program for it by giving the gdb-server address(es) and relevant data.
> [...]
> GDB-servers are typically enhanced by device vendors by providing their specific algorithms and methods. GDB-server typically communicates with hardware with USB/Serial/other connections

## Put it all together

So, in order to debug, you have a couple of things to run:
- A **debugger** with interface compatible with Cortex-M's proposed interface (SWD, JTAG, etc...)
	- Examples: [[#Some debuggers]] 
- A **gdb-server** in order to set debugger available to gdb and translate gdb commands into target-specific commands. Some debuggers like JLink already embeds it all together and you just have to start debug session
	- Examples: STLinkServer, OpenOCD, JLinkGDBServer
- The final part: **gdb program** which will debug, flash and/or manipulate the program onto the target, through gdb-server, with the hardware help of the debugger
	- Examples: arm-none-eabi-gdb, gdb-multiarch

>[!TIP] More than that
>There are many ways to communicate with DAP and CoreSight of a Cortex-M, like debugging with a UART communication only. But there are more advanced topic which we will not discuss in this article

>[!TIP] gdb commands
>To see what kind of command you may execute, look at gdb documentation. You may find some interesting commands in [GDB Commands — Black Magic Debug documentation](https://black-magic.org/usage/gdb-commands.html) and some other in [GDB Documentation](https://www.sourceware.org/gdb/documentation/)


How to set debugger is quite easy and also quite different, depending on the target to be installed. TO understand how is it done, please refer to your hardware target documentation, as well as the debugger documentation

How to set gdb-server is one of the biggest part explained in [[External sources and program to install]]. I mostly focus my work on OpenOCD as it is the most versatile of all, open source and used by all targets I had. 

For the gdb program, I mainly used arm-none-eabi-gdb, from the last official release of gnu-arm-none-eabi. GDB multiarch also works fine, but may be old and not support some more recent SoC.

## Some debuggers
### Debug with STLink debugger 

**Characteristics:**
- Works fine with the following gdb-server: `st-util`, `OpenOCD` and `ST-LINK server`
- ST's official `ST-LINK server` works only on Windows though
- Integrated on the PCB of almost all NUCLEO and DISCOVERY kits
- Cheap hardware (25-50$)
- Virtual COM port available (get `putchar()` over UART)

### Debug with Black Magic debug probe

**Website:** [Black Magic Debug: The Plug&Play MCU Debugger — Black Magic Debug documentation](https://black-magic.org/index.html)

**Characteristics:**
- Fully open source, hardware and software
	- Debugger's software may be flashed onto other boards which will be used as debugger (for example, it is possible to use an STMF4-Discovery board as the debugger's hardware)
	- Create your own debugger
	- Port new architectures or new microcontrollers
- Supports a lot of architecture
- MIPI-10 interface, as well as UART port (get `putchar()` over UART)
- Automation of flash and debugging with scripts
- Relatively cheap: ~70-90$

### Debug with J-Link

Todo

### Debug with Raspberry Debug Probe

Todo