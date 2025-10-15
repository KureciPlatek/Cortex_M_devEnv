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
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
#include <stdio.h>

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

#ifdef USE_SWO
/**
 * @brief redefine _write function in case of SWO use with BMP debugger
 *
 * @param file
 * @param ptr
 * @param len
 * @return
 */
//int _write(int file, char *ptr, int len);

// #define SWO_FREQ 115200
// #define HCLK_FREQ 216000000
//
// /* Pelican TPIU
//  * cut down TPIU implemented in STM32F7 series
//  * see en.DM00224583.pdf page 1882
//  */
// #define TPIU_CURRENT_PORT_SIZE *((volatile unsigned *)(0xE0040004))
// #define TPIU_ASYNC_CLOCK_PRESCALER *((volatile unsigned *)(0xE0040010))
// #define TPIU_SELECTED_PIN_PROTOCOL *((volatile unsigned *)(0xE00400F0))
// #define TPIU_FORMATTER_AND_FLUSH_CONTROL *((volatile unsigned *)(0xE0040304))
#endif

/* Private defines -----------------------------------------------------------*/
#define LED_GREEN_Pin         GPIO_PIN_0
#define LED_GREEN_GPIO_Port   GPIOB
#define LED_RED_Pin           GPIO_PIN_14
#define LED_RED_GPIO_Port     GPIOB
#define DBG_SWO_Pin           GPIO_PIN_3
#define DBG_SWO_GPIO_Port     GPIOB

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
