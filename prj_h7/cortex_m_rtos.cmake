# Cmake subfile to list required inputs for STM32's ThreadX library

if (DEFINED RTOS_THREADX)
   message(STATUS  "Selected RTOS: ThreadX")

#   set (THREADX_SECURE) # ThreadX may be compiled as secure, in case we have a CortexM with TrustZone

   set (THREADX_PATH ${PRJ_EXTSRC_PATH}/threadx)
   message(STATUS  "THREADX PATH: ${THREADX_PATH}")

   # Includes required by ThreadX
   set(THREADX_INCLUDE_DIR
      ${THREADX_PATH}/common/inc
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/inc
   )

   # Required?
   # set(TX_USER_FILE  ${CMAKE_SOURCE_DIR}/app/tx_user.h)

   # List all source and assembly files required by ThreadX
   # @todo maybe change for exaustive and fixed list of source files (avoid file with GLOB)
   file (GLOB_RECURSE THREADX_SRC CONFIGURE_DEPENDS
      ${THREADX_PATH}/common/src/*.c
   )

   set (THREADX_PORT_SRC
   # No TrustZone and security requirement for ThreadX on H7
   #   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/txe_thread_secure_stack_allocate.c
   #   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_initialize_low_level.S   # not this one as it is rewritten in project (avoid modifs on external project)
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_misra.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_context_restore.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_context_save.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_interrupt_control.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_interrupt_disable.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_interrupt_restore.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_schedule.S
   # No TrustZone and security requirement for ThreadX on H7
   #   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack_allocate.S
   #   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack.c
   #   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack_free.S
   #   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack_initialize.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_stack_build.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_system_return.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_timer_interrupt.S
   )

   # Sources required by ThreadX
   set(THREADX_PROJECT_SOURCES
      ${THREADX_SRC}
      ${THREADX_PORT_SRC}
   )
elseif(DEFINED RTOS_FREERTOS)
   message(STATUS  "Selected RTOS: FreeRTOS")

   # Pull in FreeRTOS (must be before project)
   if(NOT DEFINED FREERTOS_KERNEL_PATH)
      set(FREERTOS_KERNEL_PATH ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel)
      message(STATUS "FREERTOS_KERNEL_PATH = ${FREERTOS_KERNEL_PATH}")
   endif()
   include(FreeRTOS_Kernel_import.cmake)

else()
   message (WARNING "No RTOS selected")
endif()