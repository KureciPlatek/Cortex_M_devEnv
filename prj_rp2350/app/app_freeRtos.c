/**
 * @file    app_freeRtos.c
 * @author  Jeremie Gallee (jeremie.gallee@noser.com)
 * @brief   FreeRTOS application for project prj_rp2350
 *          Creates some tasks and have fun
 * @version 0.1
 * @date    2025-08-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "app_freeRtos.h"
#include "pico/async_context_freertos.h"
#include "pico/cyw43_arch.h"

/**
 * @brief Simple blinky FreeRTOS task
 * @param params set of params if required
 */
void task_threadOne(void *params)
{
   while (1)
   {
      printf("FreeRTOS thread one\n");
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
      vTaskDelay(250);
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
      vTaskDelay(250);
   }
}

void freeRtos_init(void)
{
   TaskHandle_t task;

   /* This situation is for a SMP STATIC configuration of FreeRTOS */
   static StackType_t main_stack[TASK_ONE_STACK_SIZE];
   static StaticTask_t main_buf;
   task = xTaskCreateStatic(task_threadOne, "taskOne", TASK_ONE_STACK_SIZE, NULL, TASK_ONE_PRIORITY, main_stack, &main_buf);

   vTaskCoreAffinitySet(task, 1);

   /* Start the tasks and timer running. */
   vTaskStartScheduler();
}