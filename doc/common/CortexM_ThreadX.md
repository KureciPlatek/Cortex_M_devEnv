
# Structure of ThreadX Git repo

## ThreadX source and ports folders

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

## ThreadX git repo other folders

| Folder    | Description                                                                                                                                                    |
| --------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `docs`    | As it said                                                                                                                                                     |
| `utility` | Some files to be used as utilities, like having compatibility to other RTOS (FreeRTOS, POSIX, OSEK). Look at execution profile, or enter low poser mode        |
| `script`  | Some scripts used for many different stuff. Quite fuzzy, forget it                                                                                             |
| `samples` | Contain a one C file which is an example how to use ThreadX on a `main()` function                                                                             |
| `cmake`   | Contains CMake toolchains (For Cortex_M3, M4, M7, M0, Linux and win32 only, no Cortex-A or Cortex-R toolchain definitions)<br>None are using `utilities.cmake` |
| `test`    | Contains a whole test environment, like regression tests. Don't care, it is for ThreadX developpers only                                                       |
| `build`   | Generated when "cmaking" the project with command mentioned before                                                                                             |

## Folder `ports`

When looking at `/ports/cortex_m4/gnu/*` we can see that the readme.txt tells us to run the build_threadx.bat But as we suppose, it is the `/scripts/build_tx.sh` which has the following effect:

In `scripts/build_tx.sh` -> it will call `/test/tx/cmake/run.sh` -> which will run `/scripts/cmake_bootstrap.sh`

Which do some stuff...


# Integrate ThreadX on Cortex-Mx microcontrollers

## Creation and use of a library

Idea: have an external or git sub-module folder with official ThreadX repo from Git. 
Compile it as a static library OR compile it with source of project

Azure ThreadX Git repo:

```bash
$ git clone https://github.com/eclipse-threadx/threadx.git
```

compile ThreadX from its Git repo:

```bash
$ cmake -Bbuild -DCMAKE_TOOLCHAIN_FILE=cmake/cortex_m4.cmake .
$ cmake --build ./build
```

>[!TIP]
>Commando option `-GNinja` was removed as we avoid Window's development environment

It creates `/build/libthreadx.a` of 200 kB.

## Add source to a project 

It is also possible to integrate all sources directly to project and compile it with the project instead of having a library. For that, a `<your_preferred_name>.cmake` file which lists all required files from `common/` and `ports/` folders:

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

CMake variable `${THREADX_PROJECT_SOURCES}` can then be added to `${PROJECT_SOURCES}` variable in `CMakeLists.txt`.
Variables explanations:
- `${MCPU_PATH}` is the cortex-M path name in ThreadX folder, for example `cortex_m4` be sure to have it defined in your `CMakeLists.txt`
- `${THREADX_PATH}` is the path to your ThreadX source folder, be sure to have it defined in your `CMakeLists.txt`


>[!NOTE]
>`tx_initialize_low_level.S` was removed as it is already integrated in our project source file, like some others. This file describes in assembler very important and core interrupts like `SysTick_Handler`:
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


>[!WARNING] 
>Something still to see if it required in CMake environment:
>```cmake
># Required?
set(TX_USER_FILE  ${CMAKE_SOURCE_DIR}/app/tx_user.h)
>```

