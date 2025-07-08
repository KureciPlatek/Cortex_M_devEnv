/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"
/* Private includes ----------------------------------------------------------*/
#include "stdio.h"

#define LED_RED_GPIO_Port     GPIOG
#define LED_RED_Pin           GPIO_PIN_10
#define LED_GREEN_GPIO_Port   GPIOG
#define LED_GREEN_Pin         GPIO_PIN_0

void Error_Handler(void);

#endif /* MAIN_H */

