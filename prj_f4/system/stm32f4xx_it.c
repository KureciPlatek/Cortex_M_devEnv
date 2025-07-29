/**
 ******************************************************************************
 * @file    stm32f4xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
   while (1) {}
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
   while (1) {}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
   while (1) {}
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
   while (1) {}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
   while (1) {}
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles TIM2 global interrupt
 */
void TIM2_IRQHandler(void)
{
   if ((htim2.Instance->SR & (TIM_FLAG_UPDATE)) == (TIM_FLAG_UPDATE))
   {
      /* Too heavy HAL function to increment HAL_Tick. Call stm32's MCU HAL tick incrementer directly */
      //   HAL_TIM_IRQHandler(&htim2);
      __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
      HAL_IncTick();
   }
}
