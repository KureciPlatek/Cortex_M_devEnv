# Cmake subfile to list required inputs for STM32's HAL
set (STMCUBE_HAL_PATH ${PRJ_EXTSRC_PATH}/STM32Cube${MCU_FAMILY_UPPERCASE}/Drivers)
# message("STM32 HAL path:" ${STMCUBE_HAL_PATH})

set(HAL_INCLUDE_DIR
   ${STMCUBE_HAL_PATH}/CMSIS/Device/ST/${MCU_FAMILY}/Include
   ${STMCUBE_HAL_PATH}/CMSIS/Include
   ${STMCUBE_HAL_PATH}/${MCU_FAMILY}_HAL_Driver/Inc
)

# Set source path for some UTF-8 economy below
set(STMCUBE_HAL_SRC_PATH ${STMCUBE_HAL_PATH}/${MCU_FAMILY}_HAL_Driver/Src)
# Add required HAL files for project (avoid list all and compile unwanted files)
set (HAL_PROJECT_SOURCES
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_cortex.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_rcc.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_rcc_ex.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_gpio.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_dma.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_dma_ex.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_pwr.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_pwr_ex.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_uart.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_tim.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_tim_ex.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal.c
   ${STMCUBE_HAL_SRC_PATH}/stm32${MCU_FAMILY_LOWERCASE}xx_hal_exti.c
)
