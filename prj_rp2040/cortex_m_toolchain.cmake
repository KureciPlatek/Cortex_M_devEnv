# arm-toolchain.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Variables to sort out which microcontroller family we are compiling for. It will help for path
set (UPPERCASE_MCU_FAMILY_H7 H7) # STMicroelectronics STM32H7xx
set (LOWERCASE_MCU_FAMILY_H7 h7) # STMicroelectronics STM32H7xx
set (UPPERCASE_MCU_FAMILY_F4 F4) # STMicroelectronics STM32H7xx
set (LOWERCASE_MCU_FAMILY_F4 f4) # STMicroelectronics STM32H7xx
set (UPPERCASE_MCU_FAMILY_N6 N6) # STMicroelectronics STM32N6xx
set (LOWERCASE_MCU_FAMILY_N6 n6) # STMicroelectronics STM32N6xx
set (UPPERCASE_MCU_FAMILY_RP RP) # Raspberry RP2040 / RP2350
set (LOWERCASE_MCU_FAMILY_RP rp) # Raspberry RP2040 / RP2350

# Variables to sort Cortex-M architecture family
set (MCPU_CORTEX_M55       "cortex-m55")
set (MCPU_CORTEX_M55_PATH  cortex_m55)
set (MCPU_CORTEX_M7        "cortex-m7")
set (MCPU_CORTEX_M7_PATH   cortex_m7)
set (MCPU_CORTEX_M4        "cortex-m4")
set (MCPU_CORTEX_M4_PATH   cortex_m4)
set (MCPU_CORTEX_M0        "cortex-m0")
set (MCPU_CORTEX_M0_PATH   cortex_m0)
set (MCPU_CORTEX_M33       "cortex-m33")
set (MCPU_CORTEX_M33_PATH  cortex_m33)

# COmpiling options for Cortex-M architecture (if the architecture supports it)
set (MFLOAT_ABI_SOFT       "soft")
set (MFLOAT_ABI_HARD       "hard")
set (MFLOAT_ABI_MIX        "softfp")
set (MFPU_FPV4_SP_D16      "fpv4-sp-d16")
set (MFPU_FPV5_D16         "fpv5-d16")
set (RUNTIME_LIB_SYSMEM    "nano.specs")
set (RUNTIME_LIB_SYSCALLS  "nosys.specs")
# set (SECURE_CODE           "-mcmse")   # for Cortex-M with TrustZone only (M7, M55 and M33)

# Set path to folder with all required libraries and external sources. 
message("Running on ${CMAKE_HOST_SYSTEM_NAME}")
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
   message(" --- LINUX ---")
   set (PRJ_EXTSRC_PATH $ENV{HOME}/work)  # Go to home in Linux and start path from there
endif()
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
   message(" --- WINDOWS ---")
   # Sadly $ENV{HOMEPATH} returns path with backslashes, which is not supported by CMake (\U not known).
   # So I have to put my own absolute path. Please modify it for yours
   set (PRJ_EXTSRC_PATH C:/Users/jeremie.gallee/Documents/Project_CortexM_blog)
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

# Prevent CMake from testing the compiler in trying to compile a simple executable (otherwise it will crash)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Search for programs in the host environment, libraries and headers in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

