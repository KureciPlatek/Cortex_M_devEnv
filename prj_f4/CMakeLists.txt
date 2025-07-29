cmake_minimum_required(VERSION 3.20)
# Be sure to have configured your PATHs for STM32 HAL/LL drivers and ThreadX in the following file:
include(${CMAKE_SOURCE_DIR}/gnu_arm_toolchain.cmake)

# To move to .cmake
set (MCPU_CORTEX_M55       "cortex-m55")
set (MCPU_CORTEX_M55_PATH  cortex_m55)
set (MCPU_CORTEX_M7        "cortex-m7")
set (MCPU_CORTEX_M7_PATH   cortex_m7)
set (MCPU_CORTEX_F4        "cortex-m4")
set (MCPU_CORTEX_F4_PATH   cortex_m4)

# --- Project name
set (MCU_FAMILY_UPPERCASE ${UPPERCASE_MCU_FAMILY_F4})
set (MCU_FAMILY_LOWERCASE ${LOWERCASE_MCU_FAMILY_F4})
set (CMAKE_PROJECT_NAME prj_${MCU_FAMILY_LOWERCASE})
project(${CMAKE_PROJECT_NAME} C CXX ASM)

# --- Project/mcu target variables
# All cmake required variables which are project specific AND NOT CMake standard variables
# Do it first as it sets Makefile generation options for rest of this CMakeLists.txt
# @todo: Add switch/condition to check which arch/target we are building for
set (MCU_FAMILY STM32${MCU_FAMILY_UPPERCASE}xx)
set (MCPU       ${MCPU_CORTEX_F4})  # depends on target and required compiling options
set (MCPU_PATH  ${MCPU_CORTEX_F4_PATH})  # depends on target and required compiling options
set (MFPU       ${MFPU_FPV4_SP_D16}) # depends on target and required compiling options
set (MFLOAT_ABI ${MFLOAT_ABI_HARD})  # depends on target and required compiling options

# --- Includes
# Add switch/condition to check which arch/target we are building for
set (STARTUP_SCRIPT ${CMAKE_SOURCE_DIR}/startup/startup_stm32f411xe.s)

# --- External modules and sources
# Add HAL list of source and include
include(${CMAKE_SOURCE_DIR}/stm32_hal_srcList.cmake)

# Add ThreadX list of source and include
include(${CMAKE_SOURCE_DIR}/stm32_threadx_srcList.cmake) 

set(INCLUDE_DIR
   ${CMAKE_SOURCE_DIR}/app
   ${CMAKE_SOURCE_DIR}/lib
   ${CMAKE_SOURCE_DIR}/system
   ${CMAKE_SOURCE_DIR}/startup
   ${HAL_INCLUDE_DIR}
   ${THREADX_INCLUDE_DIR}
)

# --- Sources
# Add switch for target
# Improvement: use target_sources() to avoid specifing those at add_executable() cmd
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
set (LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/link/stm32f411retx_flash.ld)

# --- Project preprocessors
add_compile_definitions(
   STM32F411xE
   USE_FULL_LL_DRIVER
   USE_HAL_DRIVER
)

# --- Project compilation flags/parameters
# Possible: add_compile_options()
set (CMAKE_C_FLAGS 
   "-mcpu=${MCPU} -std=gnu17 -mfpu=${MFPU} -mfloat-abi=${MFLOAT_ABI} --specs=${RUNTIME_LIB_SYSMEM} --specs=${RUNTIME_LIB_SYSCALLS} ${SECURE_CODE} -mthumb -Wall -Werror -ggdb3 -O0"
#   "-mcpu=${MCPU} -std=gnu17 -mfpu=${MFPU} -mfloat-abi=${MFLOAT_ABI} ${RUNTIME_LIB_SYSMEM} -mthumb -specs=nano.specs -specs=nosys.specs -Wall -O2 -g -ffunction-sections -fdata-sections"
)

# --- Project assembler flags/parameters
set (CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")

# --- Project link flags/parameters
# Possible: add_link_options()
set (CMAKE_EXE_LINKER_FLAGS 
   "-mcpu=${MCPU} -mfpu=${MFPU} -mthumb -T${LINKER_SCRIPT} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections"
   #"-T${LINKER_SCRIPT} -specs=nano.specs -specs=nosys.specs -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections -static -Wl,--start-group -lc -lm -Wl,--end-group"
)

# --- Create project's Makefile
add_executable(${CMAKE_PROJECT_NAME}.elf ${PROJECT_SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME}.elf PUBLIC ${INCLUDE_DIR})


# Improvements:
# add_custom_command() for hex and bin files
# add_custom_command() to flash automatically and start debug session if debug?
# Add command to flas over STLink or start with debugger and so on (see later)


