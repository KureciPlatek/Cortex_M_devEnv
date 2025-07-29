# gnu-arm-toolchain.cmake
# Set GNU-ARM toolchain parameters, path, variables and so on 
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Variables to sort out which STM32 family we are compiling for
set (UPPERCASE_MCU_FAMILY_H7 H7)
set (LOWERCASE_MCU_FAMILY_H7 h7)
set (UPPERCASE_MCU_FAMILY_N6 N6)
set (LOWERCASE_MCU_FAMILY_N6 n6)
set (UPPERCASE_MCU_FAMILY_F4 F4)
set (LOWERCASE_MCU_FAMILY_F4 f4)

# Some GNU-ARM compiler options
set (MFLOAT_ABI_SOFT       "soft")
set (MFLOAT_ABI_HARD       "hard")
set (MFLOAT_ABI_MIX        "softfp")
set (MFPU_FPV4_SP_D16      "fpv4-sp-d16")
set (MFPU_FPV5_D16         "fpv5-d16")
set (RUNTIME_LIB_SYSMEM    "nano.specs")
set (RUNTIME_LIB_SYSCALLS  "nosys.specs")
# set (SECURE_CODE           "-mcmse")   # On H7, target CPU does not support ARMv8-M Security Extensions


# Set your compiler path here
message("Running on ${CMAKE_HOST_SYSTEM_NAME}")
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
   message(STATUS " --- LINUX ---")
   set (PRJ_EXTSRC_PATH $ENV{HOME}/work)  # Go to home in Linux and start path from there
endif()
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
   message(STATUS " --- WINDOWS ---")
   # Sadly $ENV{HOMEPATH} returns path with backslashes, which is not supported by CMake (\U not known).
   # So I have to put my own absolute path. Please modify it for yours
   set (PRJ_EXTSRC_PATH  ${CMAKE_SOURCE_DIR}/../..)
endif()
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
   message(FATAL_ERROR "macOS Not supported yet")
endif()

message(STATUS "External source path: ${PRJ_EXTSRC_PATH}")

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

