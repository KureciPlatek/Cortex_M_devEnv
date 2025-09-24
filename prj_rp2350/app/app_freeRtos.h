/**
 * @file    app_freeRtos.h
 * @author  Jeremie Gallee (jeremie.gallee@noser.com)
 * @brief   FreeRTOS application for project prj_rp2350
 *          Creates some tasks and have fun
 * @version 0.1
 * @date    2025-08-08
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef APP_FREERTOS_H
#define APP_FREERTOS_H

#include "pico/cyw43_arch.h"
#include "task.h" /* FreeRTOS */
#include "mqtt_pico2w.h"

/* Define priorities of tasks */
#define TASK_ONE_PRIORITY  (tskIDLE_PRIORITY + 1UL)
#define TASK_ONE_STACK_SIZE configMINIMAL_STACK_SIZE /* FreeRTOS_config */
#define TASK_TWO_PRIORITY  (tskIDLE_PRIORITY + 1UL)
#define TASK_TWO_STACK_SIZE configMINIMAL_STACK_SIZE /* FreeRTOS_config */
#define TASK_THREE_PRIORITY  (tskIDLE_PRIORITY + 1UL)
#define TASK_THREE_STACK_SIZE configMINIMAL_STACK_SIZE /* FreeRTOS_config */
#define GYRO_MEAN_SIZE 64 /* Size of average mean of gyroscope values */

/**
 * @brief  Init and start FreeRTOS
 *         Also, init, creates and handle MQTT in an asynchrone context
 * @param  mqttComm_handle  MQTT_CLIENT_DATA_T handle to MQTT client
 */
void freeRtos_init(MQTT_CLIENT_DATA_T *mqttComm_handle);

#endif /* APP_FREERTOS_H */
