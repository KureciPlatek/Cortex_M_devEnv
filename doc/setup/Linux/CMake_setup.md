
>[!Abstract]
>I used `CMake` to manage project as it is quite powerful to configure required sources, external sources, compiling options, and so on. It is also quite used in the industry. It also allows to spare time and offers more flexibility than `Makefiles`
>
>This section explains how I configured and used `CMake` for all project examples, `rp2xx` and `stm32xx` families.

>[!INFO]
>How I organized `CMake` could **_definitely_** be modified.


My aim was to create one style of `CMakeLists.txt` that only requires few changes for all projects and also to keep it as simple and small as possible.

Well in reality, some family of microcontrollers are too different and project management is too different for all projects to be managed with one CMakeLists.txt (as said in the introduction).

So I did two `CMakeLists.txt`, one for STM32 family and one for RP family (Raspberry RP2040 and RP2350). And each project will have its own CMakeLists.txt


>[!INFORMATION] My philosophy about `CMake`
>There is a trend to have a lot of `CMakeLists.txt` for each project, sub-projects, folders and so on, which all calls each-other.
>
>I don't want that.
>
>I want to keep **_one_**, single, simple `CMakeLists.txt` for **_one_** project. No modularity of many `CMakeLists.txt` everywhere. Which on my opinion increases tight coupling (like it is done for examples of pico-sdk), instead of keeping things simple. 
>
>Why I consider this way to increase tight coupling? Simply because all those `CMakeLists.txt` are at the end depend from each other and you can't just have a simple, open-closed project with one `CMakeLists.txt`, independent from the others...


The role of those CMakeLists.txt files will be to:
 - Define project
 - Set compiler and environment in a `sub.cmake` file
 - Define path to different external sources (HAL and `ThreadX`) in `sub.cmake` files
 - Define sources of our project
 - Set all compilation flags

For each project, we will find the following files for `CMake`:

```javascript
📁 prj_xx/
├── 📁 .../
├── 📄 ...
├── 📄 CMakeLists.txt // (main CMake file)
├── 📄 cortex_m_stm32_hal.cmake // (configure HAL sources for stm32xx family - stm32xx only)
├── 📄 
├── 📄 cortex_m_toolchain.cmake // (configure GNU Arm toolchain)
└── 📄 pico_sdk_import.cmake // (import pico-sdk - rp2xxx only)
```


# Structure common for all projects

### `cortex_m_toolchain.cmake`

This file describe the different Cortex-M compile options for GNU, as well as which architecture is used.

But most important of it, it defines the Arm-GNU compiler, its path, its different tool-suite and so on. it also set paths depending on which OS is used (Windows or Linux).

For easiness in `CMakeLists.txt` and, because files and `STM32CubeHAL` folders names are alike for both STM32 families, I took the liberty to write the same `CMakeLists.txt` for all STM32 target project. The only change will be between letters describing the family, like H7 and F4. I defined those string as variables in `CMake`. I also did it for `rp2xxx` family but it is at the end useless:

```CMake
# Variables to sort out which STM32XX or RP2XXX family we are compiling for
set (UPPERCASE_MCU_FAMILY_H7 H7) # STMicroelectronics STM32H7xx
set (LOWERCASE_MCU_FAMILY_H7 h7) # STMicroelectronics STM32H7xx
set (UPPERCASE_MCU_FAMILY_N6 N6) # STMicroelectronics STM32N6xx
set (LOWERCASE_MCU_FAMILY_N6 n6) # STMicroelectronics STM32N6xx
set (UPPERCASE_MCU_FAMILY_RP RP) # Raspberry RP2040 / RP2350
set (LOWERCASE_MCU_FAMILY_RP rp) # Raspberry RP2040 / RP2350
```

Those variables will be used in 

### `cortex_m_rtos.cmake`

I made a separated `.cmake` file to manage the selected RTOS. As it could be `ThreadX` or `FreeRTOS` (for project `prj_rp2350` only)

It defines path to sources, which source will be included into project and some variables.

This script looks if we want to build our project with `FreeRTOS` or with `ThreadX` and will set the different paths, depending on this condition:

```CMake
# For ThreadX
if (DEFINED RTOS_THREADX)
   ...
   # Variable ${THREADX_PROJECT_SOURCES} will be required by CMakeLists.txt afterwards
   set(THREADX_PROJECT_SOURCES
      ${THREADX_SRC}
      ${THREADX_PORT_SRC}
   )

# For FreeRTOS   
elseif(DEFINED RTOS_FREERTOS)
   ...
   # Variable ${FREERTOS_KERNEL_PATH} will be required by CMakeLists.txt afterwards
   set(FREERTOS_KERNEL_PATH ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel)

   # this import.cmake is required by pico-sdk  
   include(FreeRTOS_Kernel_import.cmake)
endif()

```

Please take a look at the script for deeper understanding.

# For `stm32xx` family

Sub `CMake` file `cortex_m_stm32_hal.cmake` describes how import of STM32's HAL is configured.


So what I did is clone both of STM32's HAL into distinct directories, which I will give the path to `cortex_m_stm32_hal.cmake` script.


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


# For `rp2xxx` family 

Look at all description here: [[About_the_Pico-SDK]], section "Configuration of CMakeLists.txt".