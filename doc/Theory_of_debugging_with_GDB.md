### Introduction

Nothing new here, `gdb` stands for **G**NU **D**e**B**ugger. It is able to debug programs coming from many different programming languages, like C, C++ mainly, but also Fortran, Go and others. It knows how the programing language standard works, and provides an environment in which the program runs, and all information from inside the program, as well as execute it step by step, print registers and memory, value of variables, call specific functions and so on.


But **gdb** is only the debugger, it knows only to stop, run, watch, breakpoints and so on, on C/C++/ASM (and others) languages. On a machine like the laptop, it is easy because the OS is providing a full space for `gdb` to run the program in the language environment (with different sections, stack pointer and so on for people a little bit familiar with C/C++).

However, on microcontroller, hardware it is almost each time different and how to set breakpoints, look at variable, look at memory is for it unknown. There is no OS providing the environment.


That's when **gdb-server** comes in. It does this translation. `gdb` will connect to this server to start a debug session and its basic actions, like setting breakpoints, look at memory and so on will be translated by debugger, onto the target. Actually also flashing, erasing and other mandatory/useful features when working on a microcontroller are provided by this `gdb-server`

This gdb-server has to know what kind of architecture we are dealing with, used memory technology, clock frequency of debugger (STLINK, JLink...), configure its server and so on. Sometimes it is automatic, sometimes you have to configure yourself (it is the case of OpenOCD for example)


There is the last part, `gdb-server` requires a hardware part, the famous debugger, to execute all the actions (breakpoint, read/write memory, run functions...) onto the target/board, through JTAG, SWD or other interface/protocol.


**Overview of debug on external target**:
![[gdb_gdbServer_view.svg]]

**With:**
- `gdb` the debugger itself, there the user load, flashes, debugs its compiled executable. It is where we will have fun debugging
- `gdb-server` is the other program to run, it finds the debugger, communicates with it to understand what kind of target is on the other side of the SWD/JTAG interface and provides a TCP socket for `gdb` to connect on it
- The debugger itself, which translates target's debug communication to USB and is used by `gdb-server` to manipulate target
- The target itself, which offers `SWD`, `JTAG` or any other interface, which has to be understood by debugger. `SWD` is `ARM` specific, `JTAG` is supported by ARM but there are other debug interfaces
- `UART/Serial` is as shown, in dashes because it is optional. Some debuggers (even small ones) offers this very handy virtual serial interface. It is useful because one could forward `printf()` from program on target, to a `UART` interface, which will be forwarded by debugger to a virtual `/dev/` or COM port interface and tadaaaa, you see your logs live, while debugging


>[!WARNING]
>Everything is very simply said, it is in reality a little bit more complex and this diagram may become way much more complex than that. Bigger debuggers and architectures requires another level. But this is mainly the idea.


>[!TIP]
>As we now know that `gdb-server` and debugger hardware are two different entities, it should be clear that some `gdb-servers` supports some hardware, some don't and some `gdb-server` are more suited for a hardware than others. 
>
>For example STLINK-V3E works better with the official `STLinkServer` than the open-source project `stlink-org` ([GitHub - stlink-org/stlink: Open source STM32 MCU programming toolset](https://github.com/stlink-org/stlink)) 

## Put it all together

So, in order to run a debug session on a Cortex-M target, you have a couple of things to run:

|     Element      | Description                                                                                                                                                                                                 | Example                                     |
| :--------------: | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------- |
|  **`debugger`**  | with interface compatible with Cortex-M's proposed interface (SWD, JTAG, etc...)                                                                                                                            | STLink, JLink...                            |
| **`gdb-server`** | in order to set debugger available to `gdb` and translate `gdb` commands into target-specific commands. Some debuggers like `JLink` already embeds it all together and you just have to start debug session | `STLinkServer`, `OpenOCD`, `JLinkGDBServer` |
|    **`gdb`**     | which will debug, flash and/or manipulate the program onto the target                                                                                                                                       | `arm-none-eabi-gdb`, `gdb-multiarch`        |



---

# Further literature for those interested (optional)

### About `gdb-server`:

From Marus's blog ([Marus/cortex-debug: Visual Studio Code extension for enhancing debug capabilities for Cortex-M Microcontrollers](https://github.com/Marus/cortex-debug)):

> You may know them as OpenOCD, pyOCD, JLinkGDBServer, STLinkServer, etc. GDB by itself does not know anything about your device. It does know what an ARM architecture looks like but has no clue about much else. It relies on a program called a gdb-server that provides that knowledge and services. Among services, the **gdb-server helps create breakpoints**. GDB knows the instruction address of the breakpoint but does not know how to create a breakpoint in the device (especially for a FLASH-based device, which registers to set, etc.) **GDB does NOT know how to load** (program) the device with the executable file. **It asks the gdb-server to load** the program for it by giving the gdb-server address(es) and relevant data.
> [...]
> GDB-servers are typically enhanced by device vendors by providing their specific algorithms and methods. GDB-server typically communicates with hardware with USB/Serial/other connections

### `gdb` commands

To see what kind of command you may execute, look at gdb documentation. You may find some interesting commands in [GDB Commands — Black Magic Debug documentation](https://black-magic.org/usage/gdb-commands.html) and some other in [GDB Documentation](https://www.sourceware.org/gdb/documentation/)




**Next:**

| [[Theory_of_debugging_with_GDB]] < Previous page | Next page > [[how_to_setup]] |
| :----------------------------------------------- | ---------------------------: |
