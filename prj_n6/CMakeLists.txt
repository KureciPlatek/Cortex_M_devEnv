cmake_minimum_required(VERSION 3.20)
# Run this file with: $> cmake -DCMAKE_TOOLCHAIN_FILE=stm32.cmake -S . -B bin

# To move to .cmake
set (MCPU_CORTEX_M55       "cortex-m55")
set (MCPU_CORTEX_M55_PATH  cortex_m55)
set (MCPU_CORTEX_M7        "cortex-m7")
set (MCPU_CORTEX_M7_PATH   cortex_m7)
set (MFLOAT_ABI_SOFT       "soft")
set (MFLOAT_ABI_HARD       "hard")
set (MFLOAT_ABI_MIX        "softfp")
set (MFPU_FPV4_SP_D16      "fpv4-sp-d16")
set (MFPU_FPV5_D16         "fpv5-d16")
set (RUNTIME_LIB_SYSMEM    "nano.specs")
set (RUNTIME_LIB_SYSCALLS  "nosys.specs")
set (SECURE_CODE           "-mcmse")   # maybe not be required

# --- Project name
set (CMAKE_PROJECT_NAME prj_n6)
project(${CMAKE_PROJECT_NAME} C CXX ASM)

# --- Project/mcu target variables
# All cmake required variables which are project specific AND NOT CMake standard variables
# Do it first as it sets Makefile generation options for rest of this CMakeLists.txt
# @todo: Add switch/condition to check which arch/target we are building for
set (STM_EXTSRC_PATH $ENV{HOME}/work/STM32CubeN6)  # Go to home in Linux and start path from there
set (MCU_FAMILY STM32N6xx)
set (MCPU       ${MCPU_CORTEX_M55})  # depends on target and required compiling options
set (MCPU_PATH  ${MCPU_CORTEX_M55_PATH})  # depends on target and required compiling options
set (MFPU       ${MFPU_FPV4_SP_D16}) # depends on target and required compiling options
set (MFLOAT_ABI ${MFLOAT_ABI_HARD})  # depends on target and required compiling options

# --- Includes
# Add switch/condition to check which arch/target we are building for
set (STARTUP_SCRIPT ${CMAKE_SOURCE_DIR}/startup/startup_stm32n657x0hxq.s)

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
                     ${CMAKE_SOURCE_DIR}/app/stm32n6xx_hal_msp.c
                     ${CMAKE_SOURCE_DIR}/app/stm32n6xx_hal_timebase_tim.c
                     ${CMAKE_SOURCE_DIR}/app/system_stm32n6xx.c
                     ${CMAKE_SOURCE_DIR}/system/stm32n6xx_it.c
                     ${CMAKE_SOURCE_DIR}/system/syscalls.c
                     ${CMAKE_SOURCE_DIR}/system/sysmem.c
                     ${CMAKE_SOURCE_DIR}/system/tx_initialize_low_level.S
                     ${STARTUP_SCRIPT}
                     ${HAL_PROJECT_SOURCES}
                     ${THREADX_PROJECT_SOURCES}
)

# --- Linker
set (LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/link/STM32N657X0HXQ_AXISRAM2.ld)

# --- Project preprocessors
add_compile_definitions(
   STM32N657xx
   USE_FULL_LL_DRIVER
   USE_HAL_DRIVER
)

# --- Project compilation flags/parameters
# Possible: add_compile_options()
set (CMAKE_C_FLAGS 
   "-mcpu=${MCPU} -std=gnu17 -mfpu=${MFPU} -mfloat-abi=${MFLOAT_ABI} --specs=${RUNTIME_LIB_SYSMEM} --specs=${RUNTIME_LIB_SYSCALLS} ${SECURE_CODE} -mthumb -Wall -Werror"
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


STM32_SigningTool_CLI.exe -bin Tx_Thread_Creation.bin -nk -of 0x80000000 -t fsbl -o Tx_Thread_Creation-trusted.bin -hv 2.3 -dump Tx_Thread_Creation-trusted.bin

# Improvements:
# add_custom_command() for hex and bin files
# add_custom_command() to flash automatically and start debug session if debug?
# Add command to flas over STLink or start with debugger and so on (see later)


