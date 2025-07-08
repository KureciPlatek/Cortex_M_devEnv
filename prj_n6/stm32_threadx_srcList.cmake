# Cmake subfile to list required inputs for STM32's ThreadX library
set(THREADX_PATH ${STM_EXTSRC_PATH}/Middlewares/ST/threadx)

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
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/txe_thread_secure_stack_allocate.c
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/txe_thread_secure_stack_free.c
#   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_initialize_low_level.S   # not this one as it is rewritten in project (avoid modifs on external project)
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_misra.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_context_restore.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_context_save.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_interrupt_control.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_interrupt_disable.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_interrupt_restore.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_schedule.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack_allocate.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack.c
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack_free.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_secure_stack_initialize.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_stack_build.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_thread_system_return.S
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_timer_interrupt.S
)

# Sources required by ThreadX
set(THREADX_PROJECT_SOURCES
   ${THREADX_SRC}
   ${THREADX_PORT_SRC}
   ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/txe_thread_secure_stack_free.c
)
