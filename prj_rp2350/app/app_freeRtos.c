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
#include "pico/cyw43_arch.h"
#include "app_freeRtos.h"
#include "drv_gyroscope.h"

/**
 * @brief Simple blinky FreeRTOS task
 * @param params set of params if required
 */
void task_thread_blink(__unused void *params)
{
   while (cyw43_arch_init() != 0)
   {
      printf("Waiting for WiFi initialization...\n");
      vTaskDelay(250);
   }

   while (1)
   {
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
      printf("[ERR]  CYW43 asynchrone context init failed\n");
      vTaskDelete(NULL);
   }

   /* Enable W-Fi on pico2_w */
   cyw43_arch_enable_sta_mode();

   /* Connect to specific Wi-Fi */
   printf("[INFO] connect to %s with %s\n", WIFI_SSID, WIFI_PASSWORD);
   int connErr = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA_TKIP_PSK, WIFI_CONN_TIMEOUT);
   if (0 != connErr)
   {
      printf("[ERR]  Failed to connect to WiFi - err %d\n", connErr);
      cyw43_arch_deinit();
      vTaskDelete(NULL);
      return;
   }
   else
   {
      printf("[INFO] Connected to Wifi - ip addr: %s\n", ipaddr_ntoa(&(netif_list->ip_addr)));
   }

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
      // Publish value of ADC
      publish_temperature(mqttHandle);
      vTaskDelay(3000);
   }
}

/**
 * @brief Compute mean value of angular velocity over the last second of the gyroscope
 *        LPR5150AL. For that, do a rolling average over 64 values
 * @param params
 */
void task_thread_gyro(void *params)
{
   uint16_t angularSpeed_X_values[GYRO_MEAN_SIZE] = {0};
   uint16_t angularSpeed_Y_values[GYRO_MEAN_SIZE] = {0};
   uint32_t angularSpeed_X_mean = 0;
   uint32_t angularSpeed_Y_mean = 0;
//   float angularSpeed_X_mean_prev = 0; /* publish angular speed only if it changed a little bit */
//   float angularSpeed_Y_mean_prev = 0; /* publish angular speed only if it changed a little bit */
   gyros_vals lpr51501al_values;
   uint8_t index = 0;
   MQTT_CLIENT_DATA_T* mqttHandle = (MQTT_CLIENT_DATA_T*)(params);

   while (true)
   {
      drv_read_gyroscope(&lpr51501al_values);
      // divide by 64. It should be divided by 64 + 1, but the error here is accepted as it is just to play
      angularSpeed_X_values[index] = lpr51501al_values.xaxis;
      angularSpeed_Y_values[index] = lpr51501al_values.yaxis;
      index++;
      if(index >= GYRO_MEAN_SIZE)
      {
         for(index = 0; index < GYRO_MEAN_SIZE; index++)
         {
            angularSpeed_X_mean += angularSpeed_X_values[index];
            angularSpeed_Y_mean += angularSpeed_Y_values[index];
         }
         angularSpeed_X_mean = angularSpeed_X_mean >> 6; // Divide by 64 (GYRO_MEAN_SIZE)
         angularSpeed_Y_mean = angularSpeed_Y_mean >> 6; // Divide by 64 (GYRO_MEAN_SIZE)
         index = 0;

//         /* Publish only if mean value changed of more than 10 10% of last value */
//         if(((float)(angularSpeed_X_mean) > angularSpeed_X_mean_prev + (0.1 * angularSpeed_X_mean_prev))
//         || ((float)(angularSpeed_X_mean) < angularSpeed_X_mean_prev - (0.1 * angularSpeed_X_mean_prev))
//         || ((float)(angularSpeed_Y_mean) > angularSpeed_Y_mean_prev + (0.1 * angularSpeed_Y_mean_prev))
//         || ((float)(angularSpeed_Y_mean) < angularSpeed_Y_mean_prev - (0.1 * angularSpeed_Y_mean_prev)))
//         {
            publish_gyro(mqttHandle, (float)(angularSpeed_X_mean), (float)(angularSpeed_Y_mean));
//         }
      }
      printf("[INFO] T_GYRO: %ld\n", angularSpeed_X_mean);
      vTaskDelay(2000); /* Convert all milliseconds */
   }
}

void freeRtos_init(MQTT_CLIENT_DATA_T *mqttComm_handle)
{
   TaskHandle_t task_blink;
   TaskHandle_t task_mqtt;
   TaskHandle_t task_gyro;

   /* This situation is for a SMP DYNAMIC configuration of FreeRTOS (dynamic seems required by MQTT) */
   /* Be careful to have set configSUPPORT_DYNAMIC_ALLOCATION to 1 in FreeRTOS config header */
   xTaskCreate(task_thread_blink, "BlinkThread", TASK_ONE_STACK_SIZE, NULL, TASK_ONE_PRIORITY, &task_blink);
   xTaskCreate(task_thread_mqtt, "mqtt_client", TASK_TWO_STACK_SIZE,  (void*)mqttComm_handle, TASK_TWO_PRIORITY, &task_mqtt);
   xTaskCreate(task_thread_gyro, "gyroscope", TASK_TWO_STACK_SIZE, (void*)mqttComm_handle, TASK_THREE_PRIORITY, &task_gyro);

   vTaskCoreAffinitySet(task_blink, 0);
   vTaskCoreAffinitySet(task_mqtt, 1);
   vTaskCoreAffinitySet(task_gyro, 1);

   /* Start the tasks and timer running. */
   vTaskStartScheduler();
}
