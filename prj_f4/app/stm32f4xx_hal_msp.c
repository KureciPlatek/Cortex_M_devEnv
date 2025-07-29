/**
 ******************************************************************************
 * @file         stm32f4xx_hal_msp.c
 * @brief        This file provides code for the MSP Initialization
 *               and de-Initialization codes.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"

/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void)
{
   __HAL_RCC_SYSCFG_CLK_ENABLE();
   __HAL_RCC_PWR_CLK_ENABLE();
}

/**
 * @brief Called by STM's HAL when HAL_UART_Init() is called.
 * Call hierarchy and philosophy could definitely be changed... Thiccccc coupling baby
 * @param huart uart to configure, should be USART2 when configuring trace for STLink (redirect of printf() to putchar() )
 */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};
   if (huart->Instance == USART2)
   {
      __HAL_RCC_USART2_FORCE_RESET();
      __asm("nop");
      __HAL_RCC_USART2_RELEASE_RESET();

      /* Peripheral clock enable */
      __HAL_RCC_USART2_CLK_ENABLE();

      __HAL_RCC_GPIOA_CLK_ENABLE();
      /**USART2 GPIO Configuration
      PA2     ------> USART2_TX
      PA3     ------> USART2_RX */
      GPIO_InitStruct.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
      GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull      = GPIO_NOPULL;
      GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   }
}

/**
 * @brief Deactivate a uart instance (called from HAL)
 * @param huart 
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
   if (huart->Instance == USART2)
   {
      /* Peripheral clock disable */
      __HAL_RCC_USART2_CLK_DISABLE();

      /* USART2 GPIO Configuration */
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);
   }
}
