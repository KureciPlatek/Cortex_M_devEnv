
I used CMake to manage project, as it offers a quite powerful way to configure required sources, external sources, compiling options, and so on. It is also quite spread in the industry and projects and allows few modifications to be used for another target.

Well in reality, some family of microcontrollers are too different and project management is too different for all projects to be managed with one CMakeLists.txt.

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

# For RP family


# On Linux


# On Windows

## MinGW and the lovely backslashes

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

