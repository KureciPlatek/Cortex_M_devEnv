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

#include "board.h"
#include "task.h" /* FreeRTOS */

/* Define priorities of tasks */
#define TASK_ONE_PRIORITY  (tskIDLE_PRIORITY + 1UL)
#define TASK_ONE_STACK_SIZE configMINIMAL_STACK_SIZE /* FreeRTOS_config */
#define TASK_TWO_PRIORITY  (tskIDLE_PRIORITY + 1UL)
#define TASK_TWO_STACK_SIZE configMINIMAL_STACK_SIZE /* FreeRTOS_config */

/**
 * @brief  Init and start FreeRTOS
 * @param  none
 */
void freeRtos_init(void);

/**
 * @brief Function which will be called each time asynchrone task _async_task_worker will fire
 *        (through a timeout)
 * @param context asynchrone context of this function
 * @param worker  worker containing this function (I guess)
 */
static void async_task_function(async_context_t *context, async_at_time_worker_t *worker);

#endif /* APP_FREERTOS_H */
