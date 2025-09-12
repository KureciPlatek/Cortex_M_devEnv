/**
 * @file    board.h
 * @author  Jeremie Gallee (jeremie.gallee@noser.com)
 * @brief   Board description and specifications (is a Raspberry Pico)
 * @version 0.1
 * @date    2025-07-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "mqtt_pico2w.h"
#include "app_freeRtos.h"
#include "hardware/adc.h"

#define ADC_PIN_XAXIS   26
#define ADC_PIN_YAXIS   27
#define ADC_PIN_ITEMP   4   /* Internal temperature sensor ADC */

#endif /* BOARD_H */