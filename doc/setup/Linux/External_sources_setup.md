>[!ABSTRACT]
>Here I explain how I organized the different external/third Party source code and resources. I mainly talk about `ThreadX` and `HAL`  and how they are integrated to project with `sub-CMake.cmake` files, as for `pico-sdk` it is a little bit different and detailed in another file

# RTOS: selection of `ThreadX`

>[!HINT]
>I selected `ThreadX`, as it is quite decoupled from any technologies, unlike Zephyr which requires to install python, `venv` and other things. It offers the advantage to be just a bunch of source file I may directly integrate in my project, as I did actually.
>
>Another strong candidate is `FreeRTOS`, as it is also possible to easily integrate it to a project, by just integrating the source code. I did it actually for RP2350 project, as I wanted to add `MQTT` technology to this project, and `ThreadX` is not yet integrated to `pico-sdk` for RP2350.

## Install `ThreadX`

Download Azure `ThreadX` from its git repo wherever you want. Keep in mind that you will have to provide the `$PATH` to it, to the `CMake` files afterwards. For my projects, I put it near all other external source files, like `stm32xx`'s HAL and `pico-sdk`, in `project_blog/` root directory

```bash
git clone https://github.com/eclipse-threadx/threadx.git
```


>[!HINT]
>I could use `ThreadX` repository provided by STMicroeletronics but I wanted to do it with Azure's official repository. Maybe it is better to use `ThreadX` from STM32CubeH7's repository but I wanted to do it a different way.


#### Structure of `ThreadX` git repository

For all ported architectures (core like Cortex-M, whatever) and compiler specific files, we have the following ThreadX folders (`common`) and their corresponding `ports` which contains examples/specific files and other elements to use the `common`, onto a CPU/MCU.

| Folder           | Description                                                                                                                                            |
| ---------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `common`         | Core ThreadX files (ThreadX itself)                                                                                                                    |
| `common_modules` | Core ThreadX module files (ThreadX itself)                                                                                                             |
| `common_smp`     | Core ThreadX SMP files (ThreadX itself)                                                                                                                |
| `ports`          | Ports on some architectures (core) which can then use ThreadX, It goes from Cortex-A to Cortex-M, NXP architectures etc...                             |
| `ports_modules`  | Same as `ports`, but for architectures which can support a module style of ThreadX                                                                     |
| `ports_smp`      | Same as `ports`, but for architectures which can support an SMP (Symmetric Multi-Processing) ThreadX, mainly Cortex-A architectures                    |
| `ports_arch`     | Architecture files which are common among `ThreadX`, `Threadx SMP` and `ThreadX modules`. This is the case only for some ARMv8 and ARMv7 architectures |

>[!IMPORTANT]
>`ports` are actually partially filled with files from `ports_arch`, which contains common files for some architecture. 
>
>For example, ARMv8 share many files for `ThreadX`, `Threadx SMP` and `ThreadX modules`
>It means, that during creation of static library (or before), files from `ARMv8-M/threadx/gnu/src/*.s` will be copied to `/ports/cortex_m85/gnu/src/*.S` They are indeed the same

#### `ThreadX` git repo other folders

| Folder    | Description                                                                                                                                                    |
| --------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `docs`    | As it said                                                                                                                                                     |
| `utility` | Some files to be used as utilities, like having compatibility to other RTOS (FreeRTOS, POSIX, OSEK). Look at execution profile, or enter low poser mode        |
| `script`  | Some scripts used for many different stuff. Quite fuzzy, forget it                                                                                             |
| `samples` | Contain a one C file which is an example how to use ThreadX on a `main()` function                                                                             |
| `cmake`   | Contains CMake toolchains (For Cortex_M3, M4, M7, M0, Linux and win32 only, no Cortex-A or Cortex-R toolchain definitions)<br>None are using `utilities.cmake` |
| `test`    | Contains a whole test environment, like regression tests. Don't care, it is for ThreadX developpers only                                                       |
| `build`   | Generated when "cmaking" the project with command mentioned before                                                                                             |

### Integration to project with CMake:

I integrated all sources directly to projects and compiled it with, instead of having a library. For that, I created a `cortex_m_rtos.cmake` file which lists all required source code from `common/` and `ports/` folders of `ThreadX`'s git repository:

```cmake
# Cmake subfile to list required inputs for STM32's ThreadX library
set(THREADX_PATH ${STM_EXTSRC_PATH}/Middlewares/ST/threadx)

# Includes required by ThreadX
set(THREADX_INCLUDE_DIR
   ${THREADX_PATH}/common/inc
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/inc
)

# List all source and assembly files required by ThreadX
file (GLOB_RECURSE THREADX_SRC CONFIGURE_DEPENDS
   ${THREADX_PATH}/common/src/*.c
)

set (THREADX_PORT_SRC
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/txe_thread_secure_stack_allocate.c
   ...
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_timer_interrupt.S
#   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_initialize_low_level.S
)

# Sources required by ThreadX
set(THREADX_PROJECT_SOURCES
   ${THREADX_SRC}
   ${THREADX_PORT_SRC}
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/txe_thread_secure_stack_free.c
)
```

>[!NOTE]
>I didn't listed all required sources, as it is already in the `CMake` file. For all required sources, please refer to the `cortex_m_rtos.cmake` file

`CMake` variable `${THREADX_PROJECT_SOURCES}` can then be added to `${PROJECT_SOURCES}` variable in `CMakeLists.txt`.
Variables explanations:
- `${MCPU_PATH}` is the cortex-M path name in `ThreadX` folder, for example `cortex_m4` be sure to have it defined in your `CMakeLists.txt`
- `${THREADX_PATH}` is the path to your `ThreadX` source folder, be sure to have it defined in your `CMakeLists.txt`


>[!NOTE]
>`tx_initialize_low_level.S` was removed as it is already integrated in our project source file, like some others. 
>This file defines in assembler very important core interrupts like `SysTick_Handler`:
>```asm
>    .section .text
    .balign 4
    .syntax unified
    .eabi_attribute Tag_ABI_align_preserved, 1
    .global  SysTick_Handler
    .thumb_func
.type SysTick_Handler, function
SysTick_Handler:
...
>```
>It may be customized, like define other core IRQ handlers like `HardFault_Handler` (which could also be defined in C)
>For more information, look at explanations in [[Project_3_STM32F4_on_Linux]], section "`Threadx` modifications".


>[!WARNING] 
>Something still to see if it required in `CMake` environment:
>```cmake
># Required?
set(TX_USER_FILE  ${CMAKE_SOURCE_DIR}/app/tx_user.h)
>```


### Compile and use `ThreadX` as library

It is also possible to compile `ThreadX` separately as a library and integrate it afterwards in the project.

It could be indeed nice, but I did not do it yet. For future developments

compile `ThreadX` from its git repo:
```bash
cmake -Bbuild -DCMAKE_TOOLCHAIN_FILE=cmake/cortex_m4.cmake .
cmake --build ./build
```

It creates `/build/libthreadx.a` of approximately 200 kb.


## Third party `FreeRTOS`

#### For `stm32xx` family

Only `ThreadX` is used. `FreeRTOS` will be done in future.

#### For `rp2xxx` family

Like for other third party sources, Raspberry made its own `FreeRTOS` repository, where all porting for its chip is more up to date.

So for rp2xxx family, you have to take the following repository https://github.com/raspberrypi/FreeRTOS-Kernel.git:

```bash
git clone https://github.com/raspberrypi/FreeRTOS-Kernel.git
```

Otherwise, only RP2040 is ported at this date. `RP2350` port is not yet on `FreeRTOS` official's git repository. 

# HAL for F4 and H7

I use HAL for fast programming on `stm32xx` targets. How to use and integrate them is very easy. A deeper explanation on how it is used, how RCC and whole system is configured with it will not be explained here.

For projects to compile and programs to work, simply clone their git repositories. Like for `ThreadX`, keep in mind that the `$PATH` where you put them has to be configured or provided to `CMake` files. 

clone the following repositories:

```bash
# Get F4 family HAL and source code
git clone https://github.com/STMicroelectronics/STM32CubeF4.git
# Get H7 family HAL and source code
git clone https://github.com/STMicroelectronics/STM32CubeH7.git
```

>[!HINT]
>I downloaded the STM32Cubexx complete project. It also contains some examples and other source code, as `./Middlewares/Third_Party/ThreadX` or other submodules
>It is possible also to download only HAL, for cleaner, lighter third party code:
>`git clone https://github.com/STMicroelectronics/stm32f4xx-hal-driver.git`


### Integrate HAL to `CMake`:

As for `ThreadX`, some things have to be configured in `CMake` files, especially (or normally only)  `stm32_hal_srcList.cmake` file.

In this file I configured include path and list of required source file to compile, required by project. Here a snippet of the `CMake` file:

```cmake
# Cmake subfile to list required inputs for STM32's HAL
set (STMCUBE_HAL_PATH ${PRJ_EXTSRC_PATH}/STM32Cube${MCU_FAMILY_UPPERCASE}/Drivers)
# message("STM32 HAL path:" ${STMCUBE_HAL_PATH})

set(HAL_INCLUDE_DIR
   ${STMCUBE_HAL_PATH}/CMSIS/Device/ST/${MCU_FAMILY}/Include
   ${STMCUBE_HAL_PATH}/CMSIS/Include
   ${STMCUBE_HAL_PATH}/${MCU_FAMILY}_HAL_Driver/Inc
)

# Set source path for some UTF-8 economy below
set(STMCUBE_HAL_SRC_PATH ${STMCUBE_HAL_PATH}/${MCU_FAMILY}_HAL_Driver/Src)
# Add required HAL files for project (avoid list all and compile unwanted files)
set (HAL_PROJECT_SOURCES
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_cortex.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_rcc.c
...
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_exti.c
)
```

Again, I did not listed all source files. It is also your role as a programmer to add source files you require for your project.

For an exhaustive list of source file I required for the projects, please look at the `CMake`file.


# RP2xxx's Pico-SDK

pico-sdk is a little bit different as its install process and use imposes some things. How to set it up on you development environment, then use it on your projects with CMake is explained separately here: [[About_the_Pico-SDK]].

