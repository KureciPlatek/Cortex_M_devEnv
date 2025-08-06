# Cmake subfile to list required inputs for Cortex-M ThreadX library
# We use AzureRTOS ThreadX provided by STMicroelectronics, but any other official ThreadX repo should do it.
# @todo: get Azure ThreadX RTOS as vendor independent git repository
set (THREADX_PATH ${PRJ_EXTSRC_PATH}/x-cube-azrtos-h7/Middlewares/ST/threadx)
# message("ThreadX path:" ${THREADX_PATH})

# Includes required by ThreadX
set(THREADX_INCLUDE_DIR
   ${THREADX_PATH}/common/inc
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/inc
)

# @todo Required? It is already included in main CMakeLists.txt
# set(TX_USER_FILE  ${CMAKE_SOURCE_DIR}/app/tx_user.h)

# List all source and assembly files required by ThreadX
# @todo maybe change for exaustive and fixed list of source files (avoid file with GLOB)
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
   # TrustZone and security requirement for ThreadX on RP2350
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
