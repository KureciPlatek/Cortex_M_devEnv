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

#include "board.h" 
#include "app_freeRtos.h"
#include "pico/cyw43_arch.h"

/**
 * @brief Simple blinky FreeRTOS task
 * @param params set of params if required
 */
void task_thread_blink(__unused void *params)
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
void task_thread_mqtt(void *params)
{
   /* Init cyw43 in a FreeRTOS context */
   if (cyw43_arch_init())
   {
      printf("CYW43 asynchrone context init failed\n");
      vTaskDelete(NULL);
   }

   /* Enable W-Fi on pico2_w */
   cyw43_arch_enable_sta_mode();
   printf("[INFO] WiFi enabled\n");

   /* Connect to specific Wi-Fi */
   if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONN_TIMEOUT))
   {
      printf("[ERR] Failed to connect to WiFi");
      cyw43_arch_deinit();
      vTaskDelete(NULL);
   }
   printf("[INFO] Connected to Wifi\n");

   MQTT_CLIENT_DATA_T* mqttHandle = (MQTT_CLIENT_DATA_T*)(params);

   /* Connect to MQTT broker and start MQTT handling */
   if(true != mqtt_start_client(mqttHandle))
   {
      cyw43_arch_deinit();
      vTaskDelete(NULL);
   }
   printf("[INFO] MQTT client started\n");

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
      if(!mqttHandle->connect_done)
      {
         printf("Disconnected from MQTT broker\n");
      }
      printf("Hello from main task\n");
      // Publish value of ADC
      publish_adc(mqttHandle);
      vTaskDelay(3000);
   }
}

void freeRtos_init(MQTT_CLIENT_DATA_T *mqttComm_handle)
{
   TaskHandle_t task_blink;
   TaskHandle_t task_mqtt;

   /* This situation is for a SMP DYNAMIC configuration of FreeRTOS (dynamic seems required by MQTT) */
   /* Be careful to have set configSUPPORT_DYNAMIC_ALLOCATION to 1 in FreeRTOS config header */
   xTaskCreate(task_thread_blink, "BlinkThread", TASK_ONE_STACK_SIZE, NULL, TASK_ONE_PRIORITY, &task_blink);
   xTaskCreate(task_thread_mqtt, "mqtt_client", TASK_TWO_STACK_SIZE,  (void*)mqttComm_handle, TASK_TWO_PRIORITY, &task_mqtt);

   vTaskCoreAffinitySet(task_blink, 0);
   vTaskCoreAffinitySet(task_mqtt, 1);

   /* Start the tasks and timer running. */
   vTaskStartScheduler();
}
