# Cmake subfile to list required inputs for STM32's HAL

set(HAL_INCLUDE_DIR
   ${STM_EXTSRC_PATH}/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
   ${STM_EXTSRC_PATH}/Drivers/CMSIS/Include
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc
)

# Add required HAL files for project (avoid list all and compile unwanted files)
set (HAL_PROJECT_SOURCES
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_cortex.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_rcc.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_rcc_ex.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_gpio.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_dma.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_dma_ex.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_pwr.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_pwr_ex.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_uart.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_uart_ex.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_tim.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_tim_ex.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal.c
   ${STM_EXTSRC_PATH}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/stm32${MCU_FAMILY_LOWERCASE}xx_hal_exti.c
)
