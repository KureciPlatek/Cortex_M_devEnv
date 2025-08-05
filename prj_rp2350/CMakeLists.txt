cmake_minimum_required(VERSION 3.20)
# Be sure to have configured your PATHs for STM32 HAL/LL drivers and ThreadX in the following file:
include(${CMAKE_SOURCE_DIR}/cortex_m_env.cmake)

# To move to .cmake
set (MCPU_CORTEX_M0P       "cortex-m0+")
set (MCPU_CORTEX_M0P       "cortex-m33")
set (MFLOAT_ABI_SOFT       "soft")
set (MFLOAT_ABI_HARD       "hard")
set (MFLOAT_ABI_MIX        "softfp")
set (MFPU_FPV4_SP_D16      "fpv4-sp-d16")
set (MFPU_FPV5_D16         "fpv5-d16")
set (RUNTIME_LIB_SYSMEM    "nano.specs")
set (RUNTIME_LIB_SYSCALLS  "nosys.specs")
# set (SECURE_CODE           "-mcmse")   # On H7, target CPU does not support ARMv8-M Security Extensions

# --- Select desired project amongst those supported
set (MCU_FAMILY_UPPERCASE ${UPPERCASE_MCU_FAMILY_RP})
set (MCU_FAMILY_LOWERCASE ${LOWERCASE_MCU_FAMILY_RP})

# --- External modules and sources
message("Project RP2350")
set(PICO_BOARD pico2)   # Set pico-sdk for RP2350. Set to pico for RP2040
set(picotool_DIR /opt/picotool)  # Set picotool path (picotool was placed in /opt/ arbitrarily. Could be placed anywhere)
set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/../../pico-sdk)
include(${CMAKE_SOURCE_DIR}/pico_sdk_import.cmake)

# Add ThreadX list of source and include
#include(${CMAKE_SOURCE_DIR}/stm32_threadx_srcList.cmake) 

# --- Project name and declaration
set (CMAKE_PROJECT_NAME prj_${MCU_FAMILY_LOWERCASE})
project(${CMAKE_PROJECT_NAME} C CXX ASM)

# --- Init Pico SDK environment
pico_sdk_init()

# --- Project/mcu target variables
set (MFPU       ${MFPU_FPV4_SP_D16}) # depends on target and required compiling options
set (MFLOAT_ABI ${MFLOAT_ABI_HARD})  # depends on target and required compiling options

# --- Includes
# Hint: do not add path to pico-sdk include folders, as it will be handled later with target_link_libraries(pico_stdlib)
set(INCLUDE_DIR
   ${CMAKE_SOURCE_DIR}/app
)

# --- Sources
# Add switch for target
# Improvement: use target_sources() to avoid specifing those at add_executable() cmd
set (PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/app/main.c
#                     ${THREADX_PROJECT_SOURCES}
)

# --- Linker
# set (LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/link/STM32H723ZGTX_FLASH.ld) # Not required as already handled by PICO-SDK

# --- Project preprocessors
add_compile_definitions(
)

# --- Project compilation flags/parameters
# Possible: add_compile_options()
# set (CMAKE_C_FLAGS)          # Not required as already handled by PICO-SDK

# --- Project assembler flags/parameters
# set (CMAKE_ASM_FLAGS)        # Not required as already handled by PICO-SDK

# --- Project link flags/parameters
# set (CMAKE_EXE_LINKER_FLAGS) # Not required as already handled by PICO-SDK

# --- Create project's Makefile
add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})
# Configure some common used peripherals:
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 0)
# Add project specific libraries
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
# Add pico-sdk libraries
target_link_libraries(${CMAKE_PROJECT_NAME} pico_stdlib)
# Add .elf and .hex files
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
