cmake_minimum_required(VERSION 3.20)
message("Project STM32H7")

# Be sure to have configured your PATHs for STM32 HAL/LL drivers and ThreadX in the following file:
include(${CMAKE_SOURCE_DIR}/cortex_m_toolchain.cmake)

# --- Select desired project amongst those supported
set (MCU_FAMILY_UPPERCASE ${UPPERCASE_MCU_FAMILY_H7})
set (MCU_FAMILY_LOWERCASE ${LOWERCASE_MCU_FAMILY_H7})

# --- Set project architecture parameters
set (MCU_FAMILY STM32${MCU_FAMILY_UPPERCASE}xx)
set (MCPU       ${MCPU_CORTEX_M7})  # depends on target and required compiling options
set (MCPU_PATH  ${MCPU_CORTEX_M7_PATH})  # depends on target and required compiling options
set (MFPU       ${MFPU_FPV4_SP_D16}) # depends on target and required compiling options
set (MFLOAT_ABI ${MFLOAT_ABI_HARD})  # depends on target and required compiling options

# --- Selection of RTOS
set (RTOS_THREADX 1)   # Selection of RTOS, choose between RTOS_THREADX and RTOS_FREERTOS
include(${CMAKE_SOURCE_DIR}/cortex_m_rtos.cmake)

# --- Project name
set (CMAKE_PROJECT_NAME prj_${MCU_FAMILY_LOWERCASE})
project(${CMAKE_PROJECT_NAME} C CXX ASM)

# --- Startup file
set (STARTUP_SCRIPT ${CMAKE_SOURCE_DIR}/startup/startup_stm32h723zgtx.s)

# --- External modules and sources
include(${CMAKE_SOURCE_DIR}/cortex_m_stm32_hal.cmake)

set(INCLUDE_DIR
   ${CMAKE_SOURCE_DIR}/app
   ${CMAKE_SOURCE_DIR}/lib
   ${CMAKE_SOURCE_DIR}/system
   ${CMAKE_SOURCE_DIR}/startup
   ${HAL_INCLUDE_DIR}
   ${THREADX_INCLUDE_DIR}
)

# --- Sources
set (PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/app/main.c
                     ${CMAKE_SOURCE_DIR}/app/app_threadx.c
                     ${CMAKE_SOURCE_DIR}/app/stm32${MCU_FAMILY_LOWERCASE}xx_hal_msp.c
                     ${CMAKE_SOURCE_DIR}/app/stm32${MCU_FAMILY_LOWERCASE}xx_hal_timebase_tim.c
                     ${CMAKE_SOURCE_DIR}/app/system_stm32${MCU_FAMILY_LOWERCASE}xx.c
                     ${CMAKE_SOURCE_DIR}/system/stm32${MCU_FAMILY_LOWERCASE}xx_it.c
                     ${CMAKE_SOURCE_DIR}/system/syscalls.c
                     ${CMAKE_SOURCE_DIR}/system/sysmem.c
                     ${CMAKE_SOURCE_DIR}/system/tx_initialize_low_level.S
                     ${STARTUP_SCRIPT}
                     ${HAL_PROJECT_SOURCES}
                     ${THREADX_PROJECT_SOURCES}
)

# --- Linker
set (LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/link/STM32H723ZGTX_FLASH.ld)

# --- Project preprocessors
add_compile_definitions(
   STM32H723xx
   USE_FULL_LL_DRIVER
   USE_HAL_DRIVER
#   USE_STLINK_DEBUGGER # Uncomment for Nucleo projects that uses STLink as debugger for better clock (will use HSE from STLink->MCO instead of HSI)
)

# --- Project compilation flags/parameters
# @todo: Look if add_compile_options() is better
set (CMAKE_C_FLAGS 
   "-mcpu=${MCPU} -std=gnu17 -mfpu=${MFPU} -mfloat-abi=${MFLOAT_ABI} --specs=${RUNTIME_LIB_SYSMEM} --specs=${RUNTIME_LIB_SYSCALLS} ${SECURE_CODE} -mthumb -Wall -Werror -ggdb3 -O0"
)

# --- Project assembler flags/parameters
set (CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")

# --- Project link flags/parameters
# @todo: Look if use add_link_options() is better
set (CMAKE_EXE_LINKER_FLAGS 
   "-mcpu=${MCPU} -mfpu=${MFPU} -mthumb -T${LINKER_SCRIPT} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections"
)

# --- Create project's Makefile
add_executable(${CMAKE_PROJECT_NAME}.elf ${PROJECT_SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME}.elf PUBLIC ${INCLUDE_DIR})

