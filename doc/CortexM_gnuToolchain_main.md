# Introduction

>[!Abstract] 
>The aim of this article ist to show how to set a development environment with only **gnu-arm-none-eabi** tool-chain. The **only costs** are **laptop, Internet, electricity**, the **debuggers** (depends) and the **target boards**.

What you will learn in this article:
- Set up a **complete Cortex-M development environment** using only open-source tools for both Windows and Linux
- **Managed by CMake** (create makefile and manage sources)
- With a program that **runs ThreadX** with two tasks
- That was **flashed and debugged** with 
	- **different debuggers**  (STLink, JLink, Raspberry Pi Debug Probe)
	- **using gnu-arm-none-eabi** 
	- **different gdb-servers** (how it works will be also explained)
	- On **different Cortex-M architectures** (targets: STM32H7, STM32F4, RP2040, RP2350)


---
### Required equipment: 

#### Debuggers
I set my budget on low cost debuggers (< 80$):

|          Name           | Price          | Link                                                                                                                                                                 |
| :---------------------: | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|         STLink          | (0$ , ~60-80$) | [STLINK-V3SET - STLINK-V3 modular in-circuit debugger and programmer for STM32/STM8 - STMicroelectronics](https://www.st.com/en/development-tools/stlink-v3set.html) |
|        JLink edu        | (~65$)         | [J-Link EDU Mini](https://www.segger.com/products/debug-probes/j-link/models/j-link-edu-mini/)                                                                       |
|  Raspberry Debug Probe  | (~12$)         | [Raspberry Pi Debug Probe - Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html)                                 |
| Black Magic Debug Probe | (~80$)         | [Black Magic Debug: The Plug&Play MCU Debugger — Black Magic Debug documentation](https://black-magic.org/index.html)                                                |

#### Targets:
And I got some low cost targets too (<30$):

|           Name            | Architecture                                                  | Link                                                                                                                                                                                                        |
| :-----------------------: | :------------------------------------------------------------ | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| STM32H723ZG nucleo board  | Cortex-M7<br>big baby, even if new STM32N6 is now the captain | [NUCLEO-H723ZG - STM32 Nucleo-144 development board with STM32H723ZG MCU, supports Arduino, ST Zio and morpho connectivity - STMicroelectronics](https://www.st.com/en/evaluation-tools/nucleo-h723zg.html) |
| STM32F411RE nucleo board  | Cortex-M4                                                     | [NUCLEO-F411RE - STM32 Nucleo-64 development board with STM32F411RE MCU, supports Arduino and ST morpho connectivity - STMicroelectronics](https://www.st.com/en/evaluation-tools/nucleo-f411re.html)       |
|   Raspberry Pico RP2040   | dual Cortex-M0+                                               | [Buy a Raspberry Pi Pico – Raspberry Pi](https://www.raspberrypi.com/products/raspberry-pi-pico/)                                                                                                           |
| Raspberry Pico 2 W RP2350 | dual Cortex-M33 and dual RISC-V)                              | [Buy a Raspberry Pi Pico 2 – Raspberry Pi](https://www.raspberrypi.com/products/raspberry-pi-pico-2/)                                                                                                       |

> [!TIP]  Particularity for RP2350
> It embeds two kind of core architecture: RISC-V and Cortex-M33. Only one architecture can run at a time (obviously), it has to be selected at start-up.

---
## Roadmap

So with all that said, here is the plan of attack. From table in section [[#Different Projects with combination of targets/debuggers]] and the different theory to understand, we will have the following chapters to explore:

Hierarchy/Structure of this documentation:

```javascript
📁 project_blog/
├── 📄 CortexM_gnuToolchain_main.md (main file)[[CortexM_gnuToolchain_main]]
├── 📄 CortexM_gnuToolchain_tickets.md (list tickets and potential improvements)
├── 📄 Theory_of_debugging_with_gdb.md (understand beneath the IDE)
├── 📁 setup/
│   ├──📁 Linux/
│   │   ├── 📄 CMake_setup.md
│   │   ├── 📄 gnu_arm_toolchain_setup.md
│   │   ├── 📄 openOCD_setup.md
│   │   ├── 📄 Debuggers.md
│   │   └── 📄 external_sources_setup.md
│   ├──📁 Windows/
//  │   ├── 📄 CMake_setup.md (should be the same as in Linux)
│   │   ├── 📄 gnu_arm_toolchain_setup.md
│   │   ├── 📄 openOCD_setup.md
//  │   ├── 📄 Debuggers.md (should be the same as in Linux)
│   │   └── 📄 external_sources_setup.md
│   └── 📄 troubleshooting.md
├── 📁 projects/
│   ├── 📄 projects_main.md (information about project's choices)
│   ├── 📁 stm32/
│   └── 📁 rp2xxx/
├── 📁 setup/ (just for my diagrams, open it for fun if you want)
└── 📁 feedback_opinion/ (optional to read, nothing technical, just my opinion)
    └── 📄 my_opinion.md
```

Link to all markdown files and project folders:

- [ ] [[Theory_of_debugging_with_GDB]]
- [ ]  [[External sources and program to install]]
- [ ]  setup
	- [ ]  Linux
		- [ ] [[setup/Linux/CMake_setup]]
		- [ ] [[setup/Linux/gnu_arm_toolchain_setup]]
		- [ ] 
	- [ ]  Windows
- [ ] Set of CMake script's skeleton [[Set of CMake script skeleton]]
- [ ] How to handle and manage ThreadX's sources [[CortexM_ThreadX]]
- [ ] STM32 targets:
	- [ ] Project 1: STM32H7 on Linux [[Project_1_STM32H7_on_Linux]]
	- [ ] Project 2: STM32H7 on Windows [[Project_2_STM32H7_on_Windows]]
	- [ ] Project 3: STM32F4 on Linux [[Project_3_STM32F4_on_Linux]]
- [ ] Raspberry Pico targets:
	- [ ] Project 1: RP2040 on Linux [[Project_1_RP2040_on_Linux]]
	- [ ] Project 2:  RP2350 with Cortex-M33 on Linux and Windows [[Project 2  RP2350 with Cortex-M33 on Linux and Windows]]
	- [ ] Project 3:  RP2350 with RISC-V on Linux [[Project_3 _RP2350_with_RISC-V_on_Linux]]



**Next:**

|     | > Next page: [[Theory_of_debugging_with_GDB]] |
| :-- | --------------------------------------------: |

