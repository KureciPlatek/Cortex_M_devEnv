# arm-toolchain.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Variables to sort out which microcontroller family we are compiling for
# It will help for path and 
set (UPPERCASE_MCU_FAMILY_H7 H7) # STMicroelectronics STM32H7xx
set (LOWERCASE_MCU_FAMILY_H7 h7) # STMicroelectronics STM32H7xx
set (UPPERCASE_MCU_FAMILY_N6 N6) # STMicroelectronics STM32N6xx
set (LOWERCASE_MCU_FAMILY_N6 n6) # STMicroelectronics STM32N6xx
set (UPPERCASE_MCU_FAMILY_RP RP) # Raspberry RP2040 / RP2350
set (LOWERCASE_MCU_FAMILY_RP rp) # Raspberry RP2040 / RP2350

# Set path to folder with all required libraries and external sources. 
# I tend to keep all projects with their lib and external sources near to each other but it is purely
# an opinion and scatter those sources across your PC is also a good option.
# The idea here is to tell to CMake the most common folder for all external source, then
# in more specific_target_lib_and_sources.cmake files, define the clear path to them
# How I organized it could DEFINITELY be modified
message("Running on ${CMAKE_HOST_SYSTEM_NAME}")
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
   message(" --- LINUX ---")
   set (PRJ_EXTSRC_PATH $ENV{HOME}/work)  # Go to home in Linux and start path from there
endif()
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
   message(" --- WINDOWS ---")
   # Sadly $ENV{HOMEPATH} returns path with backslashes, which is not supported by CMake (\U not known).
   # So I have to put my own absolute path. Please modify it for yours
   set (PRJ_EXTSRC_PATH </path/to/your/project/>)
endif()
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
   message(FATAL_ERROR "macOS Not supported yet")
endif()

message(STATUS "Ext Src path: ${PRJ_EXTSRC_PATH}")

# Specify the cross-compiler
set(CMAKE_C_COMPILER    arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER  arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_OBJCOPY       arm-none-eabi-objcopy)
set(CMAKE_SIZE          arm-none-eabi-size)
set(CMAKE_LINKER        arm-none-eabi-ld)

# Prevent CMake from testing the compiler
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Search for programs in the host environment, libraries and headers in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

