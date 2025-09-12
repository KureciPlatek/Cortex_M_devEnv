/**
 * @file    drv_gyroscope.h
 * @author  Jeremie Gallee (jeremie.gallee@noser.com)
 * @brief   Driver for gyroscope LPR5150AL (obsolete)
 * @version 0.1
 * @date    2025-09-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef DRV_GYROSCOPE_H
#define DRV_GYROSCOPE_H

#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "board.h"

/**
 * @brief structure to capture gyroscope X and Y angular velocity values
 * It is given im mV/°/s so in millivolts per degree per seconds.
 */
typedef struct {
   uint16_t xaxis;
   uint16_t yaxis;
} gyros_vals;

float drv_read_temperature(void);
void drv_read_gyroscope(gyros_vals* gyrValues);

#endif /* DRV_GYROSCOPE_H */