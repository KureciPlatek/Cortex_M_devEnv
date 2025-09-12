cmake_minimum_required(VERSION 3.20)
message("Project RP2350")

# Be sure to have configured your PATHs for STM32 HAL/LL drivers and ThreadX in the following file:
include(${CMAKE_SOURCE_DIR}/cortex_m_toolchain.cmake)

# --- Select desired project amongst those supported
set (MCU_FAMILY_UPPERCASE ${UPPERCASE_MCU_FAMILY_RP})
set (MCU_FAMILY_LOWERCASE ${LOWERCASE_MCU_FAMILY_RP})

# --- Pico SDK configuration
set(PICO_BOARD pico2_w)          # Set pico-sdk's PICO_BOARD to pico2_w (RP2350 W). Other values are pico, pico_w and pico2
set(picotool_DIR /opt/picotool)  # Set picotool path (picotool was placed in /opt/ arbitrarily. Could be placed anywhere)
set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/../../pico-sdk)
include(${CMAKE_SOURCE_DIR}/pico_sdk_import.cmake)

# --- Set project architecture parameters
set (MCPU         ${MCPU_CORTEX_M33})  # depends on target and required compiling options
set (MCPU_PATH    ${MCPU_CORTEX_M33_PATH})  # depends on target and required compiling options

# --- Selection of RTOS
set (RTOS_THREADX 1)   # Selection of RTOS, choose between RTOS_THREADX and RTOS_FREERTOS
include(${CMAKE_SOURCE_DIR}/cortex_m_rtos.cmake)

# --- Project name and declaration
set (CMAKE_PROJECT_NAME prj_${MCU_FAMILY_LOWERCASE})
project(${CMAKE_PROJECT_NAME} C CXX ASM)

# --- Init Pico SDK environment
pico_sdk_init()

# --- Project/mcu target variables
set (MFPU       ${MFPU_FPV4_SP_D16}) # depends on target and required compiling options
set (MFLOAT_ABI ${MFLOAT_ABI_HARD})  # depends on target and required compiling options

# Import FreeRTOS with CMake. Take directly from FreeRTOS repo
include(${FREERSTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2350_ARM_NTZ/FreeRTOS_Kernel_import.cmake)

# --- Includes
set(INCLUDE_DIR
   ${CMAKE_SOURCE_DIR}/app
   ${CMAKE_SOURCE_DIR}/lib
)

# --- Sources
# Improvement: use target_sources() to avoid specifing those at add_executable() cmd
set (PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/app/main.c
                     ${CMAKE_SOURCE_DIR}/app/app_freeRtos.c
                     ${CMAKE_SOURCE_DIR}/lib/mqtt_pico2w.c
                     ${CMAKE_SOURCE_DIR}/lib/drv_gyroscope.c
                     ${FREERSTOS_KERNEL_PATH}/portable/MemMang/heap_4.c # FreeRTOS stack/mem management style
)

# --- Linker
# Not required as already handled by PICO-SDK

# --- Project preprocessors
add_compile_definitions(
#   configNUMBER_OF_CORES=1 # Overwrite number of cores. Set to 2 if use dual
#   configSUPPORT_STATIC_ALLOCATION=0   # When not specified, default to 0
#   configSUPPORT_DYNAMIC_ALLOCATION=1  # When not specified, default to 1
   CYW43_LWIP=1
   PICO_CYW43_ARCH_FREERTOS=1
)

# --- Project compilation flags/parameters
# Not required as already handled by PICO-SDK

# --- Project assembler flags/parameters
# Not required as already handled by PICO-SDK

# --- Project link flags/parameters
# Not required as already handled by PICO-SDK

# --- Create project's Makefile
add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})

# Configure some common used peripherals:
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 0)

# Add project specific libraries
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${INCLUDE_DIR})

# Add pico-sdk libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
   pico_stdlib
   hardware_adc
   # pico-sdk's CYW43 drivers and async context. Select between threadSafe and FreeRTOS
   pico_cyw43_arch_lwip_sys_freertos
   pico_async_context_freertos
   # pico-sdk's FreeRTOS
   FreeRTOS-Kernel-Heap4 # Could be -Static or any other heap implementation proposed by FreeRTOS
   # lwIP MQTT lib
   pico_lwip_mqtt
)

# Add .elf and .hex files
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
