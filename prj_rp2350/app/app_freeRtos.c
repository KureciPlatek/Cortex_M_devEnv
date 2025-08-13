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
#include "pico/cyw43_arch.h"
#include "mqtt_pico2w.h"

/* ASynchrone worker in which our worker (async_task_function()) will be executed */
async_at_time_worker_t _async_task_worker = {.do_work = async_task_function};

/**
 * @brief Simple blinky FreeRTOS task
 * @param params set of params if required
 */
void task_threadOne(__unused void *params)
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

/**
 * @brief Simple blinky FreeRTOS task
 * @param params set of params if required
 */
void task_threadTwo(void *params)
{
   while (1)
   {
      printf("FreeRTOS thread two\n");
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
      vTaskDelay(250);
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
      vTaskDelay(250);
   }
}

// async workers run in their own thread when using async_context_freertos_t with priority WORKER_TASK_PRIORITY
static void async_task_function(async_context_t *context, async_at_time_worker_t *worker)
{
   /* Fire our worker again so that this function is constantly refired
    with a timeout of MQTT_ASYNC_WORKER_TIMEOUT */
   async_context_add_at_time_worker_in_ms(context, worker, MQTT_ASYNC_WORKER_TIMEOUT);
   static uint32_t count = 0;
   printf("Hello from worker count=%u\n", count++);
}


void main_task(__unused void *params)
{
   /* Create async worker to create an asynchrone context (required by MQTT)*/
   async_context_t *_context = create_async_context();
   /* Add _async_task_worker to our context asynchrone "at time" context */
   async_context_add_at_time_worker_in_ms(_context, &_async_task_worker, 0);
   xTaskCreate(task_threadOne, "BlinkThread", TASK_ONE_STACK_SIZE, NULL, TASK_ONE_PRIORITY, NULL);

   /* Init cyw43 in a FreeRTOS context */
   if (cyw43_arch_init())
   {
      printf("CYW43 Wi-Fi chip init failed\n");
      return -1;
   }

   while (true)
   {
#if configNUMBER_OF_CORES > 1
      static int last_core_id = -1;
      if (portGET_CORE_ID() != last_core_id)
      {
         last_core_id = portGET_CORE_ID();
         printf("main task is on core %d\n", last_core_id);
      }
#endif
      printf("Hello from main task count=%u\n", count++);
      vTaskDelay(3000);
   }
}

void freeRtos_init(void)
{
   TaskHandle_t taskOne;
   TaskHandle_t taskTwo;

   /* This situation is for a SMP DYNAMIC configuration of FreeRTOS (dynamic seems required by MQTT) */
   /* Be careful to have set configSUPPORT_DYNAMIC_ALLOCATION to 1 in FreeRTOS config header */
   xTaskCreate(task_threadOne, "taskOne", TASK_ONE_STACK_SIZE, NULL, TASK_ONE_PRIORITY, &taskOne);
   xTaskCreate(task_threadTwo, "taskTwo", TASK_TWO_STACK_SIZE, NULL, TASK_TWO_PRIORITY, &taskTwo);

   vTaskCoreAffinitySet(taskOne, 0);
   vTaskCoreAffinitySet(taskTwo, 1);

   /* Start the tasks and timer running. */
   vTaskStartScheduler();
}
