# Introduction

>[!Abstract] 
>The aim of this article ist to show how to set a development environment with only **gnu-arm-none-eabi** tool-chain. The **only costs** are **laptop, Internet, electricity**, the **debuggers** (depends) and the **target boards**.

Short version long about the goal (read optional): [[#long short version of the aim]]

So for both Windows and Linux environment, our goal is to get a project to be:
 - **managed by CMake** (create makefile and manage sources)
 - that **runs ThreadX** with two tasks
 - that was **flashed and debugged** with 
	 - **different debuggers**
	 - **using gnu-arm-none-eabi** 
	 - **different gdb-servers** (how it works will be also explained)
	 - On **different Cortex-M architectures** (targets)

### Debuggers: 
I set my budget on low cost debuggers:

|          Name           | Price          | Link                                                                                                                                                                 |
| :---------------------: | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|         STLink          | (0$ , ~60-80$) | [STLINK-V3SET - STLINK-V3 modular in-circuit debugger and programmer for STM32/STM8 - STMicroelectronics](https://www.st.com/en/development-tools/stlink-v3set.html) |
|        JLink edu        | (~65$)         | [J-Link EDU Mini](https://www.segger.com/products/debug-probes/j-link/models/j-link-edu-mini/)                                                                       |
|  Raspberry Debug Probe  | (~12$)         | [Raspberry Pi Debug Probe - Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html)                                 |
| Black Magic Debug Probe | (~80$)         | [Black Magic Debug: The Plug&Play MCU Debugger — Black Magic Debug documentation](https://black-magic.org/index.html)                                                |
### Targets:
And I got some low cost targets too (>30$)

|           Name            | Architecture                                                  | Link                                                                                                                                                                                                        |
| :-----------------------: | :------------------------------------------------------------ | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| STM32H723ZG nucleo board  | Cortex-M7<br>big baby, even if new STM32N6 is now the captain | [NUCLEO-H723ZG - STM32 Nucleo-144 development board with STM32H723ZG MCU, supports Arduino, ST Zio and morpho connectivity - STMicroelectronics](https://www.st.com/en/evaluation-tools/nucleo-h723zg.html) |
| STM32F411RE nucleo board  | Cortex-M4                                                     | [NUCLEO-F411RE - STM32 Nucleo-64 development board with STM32F411RE MCU, supports Arduino and ST morpho connectivity - STMicroelectronics](https://www.st.com/en/evaluation-tools/nucleo-f411re.html)       |
|   Raspberry Pico RP2040   | dual Cortex-M0+                                               | [Buy a Raspberry Pi Pico – Raspberry Pi](https://www.raspberrypi.com/products/raspberry-pi-pico/)                                                                                                           |
| Raspberry Pico 2 W RP2350 | dual Cortex-M33 and dual RISC-V)                              | [Buy a Raspberry Pi Pico 2 – Raspberry Pi](https://www.raspberrypi.com/products/raspberry-pi-pico-2/)                                                                                                       |

> [!TIP]  Particularity for RP2350
> It embeds two kind of core architecture: RISC-V and Cortex-M33. Only one architecture can run at a time (obviously), it has to be selected at start-up

### Different Projects with combination of targets/debuggers:
| Target  | Platform | Debuggers        | gdb-server                      | Architecture          |
| :------ | :------- | :--------------- | :------------------------------ | :-------------------- |
| STM32H7 | Linux    | STLink/BMD/JLink | OpenOCD, st-utils               | Cortex-M7             |
| STM32H7 | Windows  | STLink/BMD/JLink | OpenOCD, st-utils, STLinkServer | Cortex-M7             |
| STM32F4 | Linux    | STLink/BMD/RDP   | OpenOCD, st-utils               | Cortex-M4             |
| RP2040  | Linux    | RDP/BMD/JLink    | OpenOCD                         | Cortex-M0+            |
| RP2350  | Linux    | RDP/BMD/JLink    | OpenOCD                         | Cortex-M33 and RISC-V |
| RP2350  | Windows  | RDP/BMD/JLink    | OpenOCD                         | Cortex-M33 and RISC-V |
I did for Windows only for target STM32H7 and RP2350 as they seems the most complex (presence of cache and MPU on Cortex-M7, dual core/drop on RP2350). I am assuming that porting on Windows for other project should be easy, as steps should be the same as those two projects.

# My environment

### Used host to work 

I mainly worked on Linux with a Raspberry Pi 5, 8 Gb RAM and quad-core Arm Cortex A76 processor.

>[!WARNING] AArch64 Linux and Pi5
>RPi5 has but only one "sliiiiiightly" disadvantage: it is aarch64 Linux, and not x86_64. Which means that many programs that were ported by vendors/companies may have been ported on Linux, but not for Raspberry Pi 5.n For example, Saleae software is not supported

### Work setup

To keep some flexibility and show that it is possible, I let my different setups (Target + Debugger) plugged to the Raspberry Pi at the office and worked remotely with VSCode and terminals through SSH. This way, I could work remotely from the office, from home or somewhere else (train, library).

VSCode offers a quite handful way to keep a GUI on remote, without requiring to forward the X server. It can indeed connects to a remote session through ssh.

>[!NOTE]
> How to work remotely with VSCode will not be explained here. Please look at https://code.visualstudio.com/docs/remote/ssh


## long short version of the aim:

We usually use all integrated development environment, generally provided by vendor or company. We just have to focus on developing and understand how the IDE works (how to create a debug session, how to configure project, how to manage external sources, etc...)
But when looking a little bit, most of the IDE (except for IAR and Keil), using gnu for ARM tool-chain, _just_ integrates some different elements/programs that may be used separately.

The aim of this article is to understand what actually lies below those IDE, because, for example, STM32CubeIDE uses all the technologies I will use, but bundles them into the big STMCubeIDE folder and forces you to uses the IDE only.

Why do that? So you could understand actually what is happening, tailor your development chain to your need, reduce price and hard-drive cost and actually increase simplicity.

For that, we will look at the debugger, the compiler mainly, and some other specific parts and look at the common point between all those projects with different targets.

### About IDE
There are IDEs like STM32CubeIDE or other Eclipse based environment, but it is not the aim here. I want to show which parts are actually important. Cleanly split the full development environment into those _parts_ and explain what they do, as much as possible how to configure/use them to tailor them to your environment.

>[!NOTE] About PlatformIO
>It is for me the same as STM32CubeIDE, it integrates it all and adds a GUI layer over it, but at the end it uses all elements that we will explain. 
>
>On my opinion, if there is the need to use a GUI to debug (which is also on my opinion quite useful), then I would go to a professional tool, like JLink and Ozone from SEGGER which covered 100% of debug cases I met until now.

### About CMake

Many projects are now managed with CMake, and on my opinion, with too much modularity. It is good to have modularity, but until a point. For example, in pico-sdk as it will be seen, CMake is on my opinion too intensively used, and managing CMake may start to take more time than developing project itself (again, on my opinion). And it should not be.

For this reason, I did not made many CMakeLists.txt, almost one for each subfolder/file like some project does. Also I did not made one CMakeLists.txt to control all different projects with different libraries and architectures, as the difference between those projects is too high to be all controlled in one CMakeLists.txt.

Instead, I strive for one single, as simple as possible CMakeLists.txt, that manages full project, one per project. Helped by two .cmake files that lists external sources and project's own sources. Not more.

>[!TIP]
>On my experience, each family of target, coming from a vendor should have its CMakeLists.txt. Indeed, each company has its own understanding of CMake, and uses as it pleases it. 
>
>**For example**: 
>I used targets from two different companies: STMicroelectronics and Raspberry Pi, and for both, different targets. For all targets from STMicroelectronics, I could keep a lot of common between the CMakeLists.txt. But almost nothing when I started to work with RP2xxx from Raspberry Pi.

## Roadmap

So with all that said, here is the plan of attack. From table in section [[#Different Projects with combination of targets/debuggers]] and the different theory to understand, we will have the following chapters to explore:

- [ ] Some theory with GDB and gdb-server [[Theory of debugging with GDB]]
- [ ] External sources and program required [[External sources and program to install]]
- [ ] Set of CMake script's skeleton [[Set of CMake script skeleton]]
- [ ] How to handle and manage ThreadX's sources [[CortexM_ThreadX]]
- [ ] STM32 targets:
	- [ ] Project 1: STM32H7 on Linux [[Project 1 STM32H7 on Linux]]
	- [ ] Project 2: STM32H7 on Windows [[Project 2 STM32H7 on Windows]]
	- [ ] Project 3: STM32F4 on Linux [[Project 3 STM32F4 on Linux]]
- [ ] Raspberry Pico targets:
	- [ ] Project 1: RP2040 on Linux [[Project 1 RP2040 on Linux]]
	- [ ] Project 2:  RP2350 with Cortex-M33 on Linux and Windows [[Project 2  RP2350 with Cortex-M33 on Linux and Windows]]
	- [ ] Project 3:  RP2350 with RISC-V on Linux [[Project 3  RP2350 with RISC-V on Linux]]




