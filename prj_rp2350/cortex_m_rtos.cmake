# Cmake subfile to list required inputs for RTOS we want to use

if (DEFINED RTOS_THREADX)
   message(STATUS  "Selected RTOS: ThreadX")

   set (THREADX_SECURE) # ThreadX may be compiled as secure, in case we have a CortexM with TrustZone

   # Get official Azure ThreadX sources from its Git repository
   set (THREADX_PATH ${PRJ_EXTSRC_PATH}/threadx)
   message(STATUS  "THREADX PATH: ${THREADX_PATH}")

   # Includes required by ThreadX
   set(THREADX_INCLUDE_DIR
      ${THREADX_PATH}/common/inc
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/inc
   )

   # @todo: Required? It is already included in main CMakeLists.txt
   # set(TX_USER_FILE  ${CMAKE_SOURCE_DIR}/app/tx_user.h)

   # List all source and assembly files required by ThreadX
   # @todo: Change for exaustive and fixed list of source files (avoid file with GLOB)
   file (GLOB_RECURSE THREADX_SRC CONFIGURE_DEPENDS
      ${THREADX_PATH}/common/src/*.c
   )

   set (THREADX_PORT_SRC
   #   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_initialize_low_level.S   # not this one as it is rewritten in project (avoid modifs on external project)
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_misra.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_context_restore.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_context_save.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_interrupt_control.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_interrupt_disable.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_interrupt_restore.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_schedule.S
      # _secure_ source files are added if we compile ThreadX as secure
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_stack_build.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_system_return.S
      ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_timer_interrupt.S
   )

   # Add secure sources of ThreadX if we compile it to use TrustZone
   if(THREADX_SECURE)
      set (THREADX_PORT_SRC
         ${THREADX_PORT_SRC}
         # TrustZone and security requirement for ThreadX on RP2350
         ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack_allocate.S
         ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack.c
         ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack_free.S
         ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack_initialize.S
      )
   endif()

   # Sources required by ThreadX
   set(THREADX_PROJECT_SOURCES
      ${THREADX_SRC}
      ${THREADX_PORT_SRC}
   )
elseif(DEFINED RTOS_FREERTOS)
   message(STATUS  "Selected RTOS: FreeRTOS")

   # Pull in FreeRTOS (must be before project)
   if(NOT DEFINED FREERTOS_KERNEL_PATH)
      set(FREERTOS_KERNEL_PATH ${CMAKE_SOURCE_DIR}/../../FreeRTOS-Kernel)
      message(STATUS "FREERTOS_KERNEL_PATH = ${FREERTOS_KERNEL_PATH}")
   endif()

else()
   message (WARNING "No RTOS selected")
endif()

