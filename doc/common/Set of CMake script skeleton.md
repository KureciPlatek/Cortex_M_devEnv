
I used CMake to manage project as it is quite powerful to configure required sources, external sources, compiling options, and so on. It is also quite spread in the industry and projects and allows few modifications to be used for another target.

Well in reality, some family of microcontrollers are too different and project management is too different for all projects to be managed with one CMakeLists.txt (as said in the introduction).

So I did two CMakeLists.txt, one for STM32 family and one for RP family (Raspberry RP2040 and RP2350)

The role of those CMakeLists.txt files will be to:
 - Define project
 - Set compiler and environment in a sub .cmake file
 - Define path to different external sources (HAL and ThreadX) in sub .cmake files
 - Define sources of our project
 - Set all compilation flags

For easiness on `CMakeLists.txt` and, because file and folder names are alike for STM32's families, I took the liberty to write the same `CMakeLists.txt` for STM32 target project, and change only between H7 and F4. I defined those two string as a variable and a switch:
```cmake
set (MCU_FAMILY_UPPERCASE ${UPPERCASE_MCU_FAMILY_H7})
set (MCU_FAMILY_LOWERCASE ${LOWERCASE_MCU_FAMILY_H7})
set (CMAKE_PROJECT_NAME prj_${MCU_FAMILY_LOWERCASE})
```

# Structure for all projects

### cortex_m_toolchain.cmake

I tend to keep all projects with their lib and external sources near to each other but it is purely an opinion and scatter those sources across your PC is also a good option. The idea here is to tell to CMake the most common folder for all external source, then in more specific_target_lib_and_sources.cmake files, define the clear path to them. How I organized it could DEFINITELY be modified.

### cortex_m_rtos.cmake


# For STM32 family

We require the HAL for each STM32's families of microcontroller: STM32H7 and STM32F4. HAL are indeed too different between STM32's families that two different set of HAL are required (you may find each HAL sources on STMicroelectronics GitHub repositories)

So what I did is clone both of STM32's HAL into distinct directories, which I will give the path to CMake scripts

For ThreadX, I could use the official ThreadX repository from Azure, but I used the one provided by STMicroeletronics.

>[!WARNING]
>I discovered that for STM32H7, ThreadX is not in the typical `<stm32_hal_git_repo>/Middlewares/ST/threadx/` like for STM32N6 or some others, but another git repo is provided by STMicroelectronics, which is named: `x-cube-azrtos-h7`. Inside this repository, **another STM32H7 HAL** is available, and **is different from the STM32CubeH7**!!
>
>After a diff, I saw only few differences, but differences anyway.


- Then on HAL sub .cmake script file:
```cmake
set (STMCUBE_HAL_PATH ${PRJ_EXTSRC_PATH}/STM32Cube${MCU_FAMILY_UPPERCASE}/Drivers)
message("STM32 HAL path:" ${STMCUBE_HAL_PATH})

...

set(STMCUBE_HAL_SRC_PATH ${STMCUBE_HAL_PATH}/${MCU_FAMILY}_HAL_Driver/Src)
set (HAL_PROJECT_SOURCES
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_cortex.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_rcc.c
...
```


#### Possible improvements:

- add_custom_command() for hex and bin files
- add_custom_command() to flash automatically and start debug session if debug?
- Add command to flas over STLink or start with debugger and so on (see later)
- Use target_sources() to avoid specifing those at add_executable() cmd
- 

# For RP family

>[!Hint] 
>Do not add path to pico-sdk include folders, as it will be handled later with target_link_libraries(pico_stdlib)


Some options to provide to pico-sdk cmake files:
`cmake -DCMAKE_BUILD_TYPE=Debug -DPICO_DEOPTIMIZED_DEBUG=1`
to avoid any optimistion

## Specificities on Linux

Nothing to declare

## Specificities on Windows

### MinGW and the lovely backslashes

To generate on Windows, thank to the "Stop thinking Windows is good", I just blocked for a couple of minutes because you have to specify to generate makefile for MinGW style (no idea why and I don't want to know why):

```bash
$ cmake .. -G "MinGW Makefiles" 
```

To make the project, you then have to run 
```bash
$ mingw32-make
```
It may surely changes if you selected another make alternative for Windows (ninja, Cygwin, ...)

>[!TIP] Stop thinking Windows is good
>They are the only one using "\\" instead of "/", which make all path useless on `CMakeLists.txt`. Yay.
>So when writing decoupled CMake files, as platform independent as possible, I wanted to use `${HOME}` environment variable, sadly, for Windows, `${HOMEPATH}` provides a path with "\\", which will generate errors. So I had to put my full path in `stm32.cmake` file.

# General CMake opinion

I find generally that people overuse CMake and its many functions for same functionality. 

>[!ERROR]
>When using two different ways, for the same goal.

For example, to add `-Wall` flag to compiler, there are different options:

```CMake
set (CMAKE_C_FLAGS "-Wall")
```

Or

```CMake
add_compile_options(-Wall)
```

**But those two ways bring different compilation results**

- In the first case, I have an error thrown:
```bash
/pico-sdk/src/rp2_common/hardware_sync_spin_lock/include/hardware/sync/spin_lock.h:152:2: error: #error no SW_SPIN_LOCK_LOCK available for PICO_USE_SW_SPIN_LOCK on this platform
  152 | #error no SW_SPIN_LOCK_LOCK available for PICO_USE_SW_SPIN_LOCK on this platform
      |  ^~~~~
```

- In the second case (from pico-sdk example), I have only warnings (a lot for an sdk on my opinion)

Git commit with the error: #51778e9

Other way to add GNU-ARM flags:
```CMake
set_source_files_properties(
    ${PICO_LWIP_PATH}/path/to/some/file.c
    PROPERTIES
    COMPILE_OPTIONS "-Wno-unused-result"
    )
```

I mean, how many ways we can do that? On my opinion, it leads to the kind of problem I encountered earlier. Too many options = too many things to manage and maintain = increased risk of non understandable problems.