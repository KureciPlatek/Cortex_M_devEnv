cmake_minimum_required(VERSION 3.20)
# Some options to provide to pico-sdk cmake files:
# cmake -DCMAKE_BUILD_TYPE=Debug -DPICO_DEOPTIMIZED_DEBUG=1 ..
set (CMAKE_BUILD_TYPE Debug)
set (PICO_DEOPTIMIZED_DEBUG 1)
# to avoid any optimistion

# Be sure to have configured your PATHs drivers and external libraries in the following file:
include(${CMAKE_SOURCE_DIR}/cortex_m_env.cmake)

# @todo To move to .cmake
set (MCPU_CORTEX_M0        "cortex-m 0")
set (MCPU_CORTEX_M0_PATH   corte_m0)
set (MCPU_CORTEX_M33       "cortex-m33")
set (MCPU_CORTEX_M33_PATH  cortex_m33)

# --- Select desired project amongst those supported
set (MCU_FAMILY_UPPERCASE ${UPPERCASE_MCU_FAMILY_RP})
set (MCU_FAMILY_LOWERCASE ${LOWERCASE_MCU_FAMILY_RP})

# --- External modules and sources
message("Project RP2350")
# --- Configure pico-sdk
set(PICO_BOARD pico2_w)
set(picotool_DIR /opt/picotool) # picotool arbitrarily set there by me
set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/../../pico-sdk)
include(${CMAKE_SOURCE_DIR}/pico_sdk_import.cmake)

# --- Set project architecture parameters
set (MCPU         ${MCPU_CORTEX_M33})  # depends on target and required compiling options
set (MCPU_PATH    ${MCPU_CORTEX_M33_PATH})  # depends on target and required compiling options

# --- FreeRTOS path
set (FREERSTOS_KERNEL_PATH ${CMAKE_SOURCE_DIR}/../../FreeRTOS-Kernel)

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
set (PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/app/main.c
                     ${CMAKE_SOURCE_DIR}/app/app_freeRtos.c
                     ${CMAKE_SOURCE_DIR}/lib/mqtt_pico2w.c
                     ${FREERSTOS_KERNEL_PATH}/portable/MemMang/heap_4.c # FreeRTOS stack/mem management style
)

# --- Project preprocessors
add_compile_definitions(
#   configNUMBER_OF_CORES=1 # Overwrite number of cores. Set to 2 if use dual
#   configSUPPORT_STATIC_ALLOCATION=0   # When not specified, default to 0
#   configSUPPORT_DYNAMIC_ALLOCATION=1  # When not specified, default to 1
   CYW43_LWIP=1
)

# --- Project compilation flags/parameters
add_compile_options(-Wall)

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
#   pico_cyw43_arch_none # LED accessible on PICO 2 W only through CYW43 Bluetooth+Wifi chip
   hardware_adc
   # pico-sdk's CYW43 drivers and async context. Select between threadSafe and FreeRTOS
#   pico_cyw43_arch_lwip_threadsafe_background
   pico_cyw43_arch_lwip_sys_freertos
   pico_async_context_freertos
   # pico-sdk's FreeRTOS
   FreeRTOS-Kernel
   FreeRTOS-Kernel-Static
   # MQTT lib
   pico_lwip_mqtt
)

# Add .elf and .hex files
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
