/**
  ******************************************************************************
  * @file    board.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the main.c
  *          file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
#include <stdio.h>

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
/* USB communication (LD1) (green/red), user LED (LD2) (below), power LED (LD3) (red) */
/* On STM32F411REx Nucleo board:
 * GREEN LED is on PA5 (SB42 soldered, SB29 not)
 * No other LED available for use, so I added one on PA4 (MORPHO Connector CN7:32)
 */
#define LED_GREEN_Pin         GPIO_PIN_5  /* GREEN led */
#define LED_GREEN_GPIO_Port   GPIOA
#define LED_RED_Pin           GPIO_PIN_4 /* POWER led */
#define LED_RED_GPIO_Port     GPIOA

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
