/**
 * @file    mqtt_pico2w.c
 * @author  Jeremie Gallee (jeremie.gallee@noser.com)
 * @brief   MQTT prototype to use with a PICO2W board (RP2350)
 *          Uses pico-sdk
 * @version 0.1
 * @date    2025-08-07
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "mqtt_pico2w.h"
#include "board.h"

/* --- Publish to topic --- */
static void pub_request_cb(__unused void *arg, err_t err);

/* --- Subscribe / unsubscribe to topics --- */
static void sub_request_cb(void *arg, err_t err);
static void unsub_request_cb(void *arg, err_t err);

/* For incoming published data on MQTT layer */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);

/* MQTT connection callback, process if success or failure */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);

/**
 * @brief Publish Pico inner temperature on MQTT_WILL_TOPIC topic
 *
 * @param mqtt_handle handle to MQTT client and management
 */
void publish_temperature(MQTT_CLIENT_DATA_T *mqtt_handle)
{
   float temperature_val = drv_read_temperature();
   char temperature_str[16];
   snprintf(temperature_str, sizeof(temperature_str), "%.2f", temperature_val);

   cyw43_arch_lwip_begin();  // Start lwIP critical section
   err_t err = mqtt_publish(mqtt_handle->mqtt_client_inst, MQTT_WILL_TOPIC,
                            temperature_str, strlen(temperature_str),
                            MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN,
                            pub_request_cb, mqtt_handle);
   cyw43_arch_lwip_end();    // End lwIP critical section

   if (ERR_OK != err)
   {
      printf("[ERR]  Publish temperature failed: %d\n", err);
   }
   else
   {
      printf("[INFO] Published: %s to %s\n", temperature_str, MQTT_WILL_TOPIC);
   }
}

void publish_gyro(MQTT_CLIENT_DATA_T *mqtt_handle, float angSpeed_X, float angSpeed_Y)
{
   // Publish adc_val on /mqtt_device_name/adc_val topic
   char XY_values_string[16];
   snprintf(XY_values_string, sizeof(XY_values_string), "%.2f - %.2f", angSpeed_X, angSpeed_Y);

   cyw43_arch_lwip_begin();  // Start lwIP critical section
   err_t err = mqtt_publish(mqtt_handle->mqtt_client_inst, MQTT_WILL_TOPIC,
                            XY_values_string, strlen(XY_values_string),
                            MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN,
                            pub_request_cb, mqtt_handle);
   cyw43_arch_lwip_end();    // End lwIP critical section

   if (ERR_OK != err)
   {
      printf("[ERR]  Publish gyro values failed: %d\n", err);
   }
   else
   {
      printf("[INFO] Published: %s to %s\n", XY_values_string, MQTT_WILL_TOPIC);
   }
}

/* ------ static methods ------ */
/**
 * @brief Callback for publishing a request
 * @param arg
 * @param err
 */
static void pub_request_cb(__unused void *arg, err_t err)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
   if (err != 0)
   {
      printf("[ERR]  pub_request_cb failed %d, %s\n", err, mqtt_handle->topic);
   }
}

/**
 * @brief Subscribe to a topic callback
 *
 * @param arg argument passed to callback function when subscribing to topic
 *            should be of type MQTT_CLIENT_DATA_T
 * @param err Error that happened to subscription request
 */
static void sub_request_cb(void *arg, err_t err)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
   if (err != 0)
   {
      printf("[ERR]  subscribe request failed %d, %s\n", err, mqtt_handle->topic);
   }
}

/**
 * @brief Unsubscribe to a topic callback
 *
 * @param arg argument passed to callback function when subscribing to topic
 *            should be of type MQTT_CLIENT_DATA_T
 * @param err Error that happened to subscription request
 */
static void unsub_request_cb(void *arg, err_t err)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
   if (err != 0)
   {
      printf("[ERR]  unsubscribe request failed %d, %s\n", err, mqtt_handle->topic);
   }
}

/**
 * @brief Callback for incoming published MQTT messages. It will save the topic of this MQTT published msg
 * @param arg     (void *) argument passed when registering this callback with mqtt_set_inpub_callback()
 * @param data    uint8_t[] array of data from MQTT published msg
 * @param len     length of data[] array
 * @param flags   some flags
 */
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;

   /* Save data from MQTT published message, its length, and add end of line at last char */
   strncpy(mqtt_handle->data, (const char *)data, len);
   mqtt_handle->len = (uint16_t)len;
   mqtt_handle->data[len] = '\0';

   /* Process data of message */
   /* Topic (mqtt_handle->topic) should have been saved before when mqtt_incoming_publish_cb() was called */
   printf("[INFO] Topic: %s, Message: %s\n", mqtt_handle->topic, mqtt_handle->data);

   /* Control LED */
   if (strcmp(mqtt_handle->topic, MQTT_LED_TOPIC) == 0)
   {
      if (lwip_stricmp((const char *)mqtt_handle->data, MQTT_LED_CMD_ON) == 0)
      {
         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
      }
      else if (lwip_stricmp((const char *)mqtt_handle->data, MQTT_LED_CMD_OFF) == 0 )
      {
         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
      }
   }
   /* Print a message and/or subscribe/unsubscribe to topic */
   else if (strcmp(mqtt_handle->topic, MQTT_PRINT_TOPIC) == 0)
   {
      /* Print message */
      printf("[INFO] received %.*s\n", len, mqtt_handle->data);

      /* Look if subscribe or unsubscribe to LED ordered */
      if(strcmp(mqtt_handle->data, MQTT_PRINT_LED_UNSUB))
      {
         /* Unsubscribe */
         printf("[INFO] Unsub to LED\n");
         mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, MQTT_LED_TOPIC, MQTT_SUBSCRIBE_QOS, (mqtt_request_cb_t)unsub_request_cb, mqtt_handle, (u8_t)0);
      }
      else if(strcmp(mqtt_handle->data, MQTT_PRINT_LED_SUB))
      {
         /* Subscribe */
         printf("[INFO] Sub to LED\n");
         mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, MQTT_LED_TOPIC, MQTT_SUBSCRIBE_QOS, (mqtt_request_cb_t)sub_request_cb, mqtt_handle, (u8_t)1);
      }
   }
}

/**
 * @brief Callback for incoming published MQTT messages. It will save the topic of this MQTT published msg
 * @param arg     (void *) argument passed when registering this callback with mqtt_set_inpub_callback()
 * @param topic   const char * of topic name (string)
 * @param tot_len length of topic name
 */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
   strncpy(mqtt_handle->topic, topic, sizeof(mqtt_handle->topic));
}



/**
 * @brief Callback function for mqtt_client_connect(). It will handle the success or failure of connection to MQTT broker
 * @param client  mqtt_client_t given back to be saved
 * @param arg     (void *) pointer to MQTT_CLIENT_DATA_T handle
 * @param status  Success or failure of MQTT connection
 */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
   printf("[INFO] mqtt conn cb\n");
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
   if (status == MQTT_CONNECT_ACCEPTED)
   {
      mqtt_handle->connect_done = true;
      mqtt_handle->mqtt_client_inst = client;
      /* Subscribe to LED topic */
      err_t err = mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, MQTT_LED_TOPIC, MQTT_SUBSCRIBE_QOS, (mqtt_request_cb_t)sub_request_cb, mqtt_handle, (u8_t)1);
      if(ERR_OK != err)
      {
         printf("[ERR]  Error while subscribe to %s: %d\n", MQTT_LED_TOPIC, err);
      }

//      /* Subscribe to MQTT_PRINT_TOPIC topic */
//      err = mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, MQTT_PRINT_TOPIC, MQTT_SUBSCRIBE_QOS, (mqtt_request_cb_t)sub_request_cb, mqtt_handle, (u8_t)1);
//      if(ERR_OK != err)
//      {
//         printf("[ERR]  Error while subscribe to %s: %d\n", MQTT_PRINT_TOPIC, err);
//      }

      // indicate online
      if (mqtt_handle->mqtt_client_info.will_topic)
      {
         mqtt_publish(mqtt_handle->mqtt_client_inst, mqtt_handle->mqtt_client_info.will_topic, "Connected", 1, MQTT_WILL_QOS, true, pub_request_cb, mqtt_handle);
      }
   }
   else if (status == MQTT_CONNECT_DISCONNECTED)
   {
      mqtt_handle->connect_done = false;
   }
   else
   {
      printf("[ERR]  Unexpected status\n");
   }
}

/**
 * @brief start client
 *
 * @param mqtt_handle
 */
bool mqtt_start_client(MQTT_CLIENT_DATA_T *mqtt_handle)
{
   const int port = MQTT_PORT;
   bool success = false;

   /* Create new mqtt client */
   mqtt_handle->mqtt_client_inst = mqtt_client_new();

   if (!mqtt_handle->mqtt_client_inst)
   {
      printf("[ERR]  MQTT client instance creation error\n");
      success = false;
   }
   else
   {
      success = true;
      printf("[INFO] IP address of this device %s\n", ipaddr_ntoa(&(netif_list->ip_addr)));
      printf("[INFO] Connecting to mqtt server at %s\n", ipaddr_ntoa(&mqtt_handle->mqtt_server_address));
   }

   if(success)
   {
      /* As lwIP API is not thread safe (function mqtt_client_connect()), we have to acquire a lock, prior to call it */
      cyw43_arch_lwip_begin();
      if (ERR_OK != mqtt_client_connect(mqtt_handle->mqtt_client_inst, &mqtt_handle->mqtt_server_address, port, mqtt_connection_cb, mqtt_handle, &mqtt_handle->mqtt_client_info))
      {
         printf("[ERR]  MQTT broker connection error\n");
         success = false;
      }
      else
      {
         printf("[INFO] connected\n");
         mqtt_set_inpub_callback(mqtt_handle->mqtt_client_inst, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, mqtt_handle);
      }
      /* Release lwIP stack lock */
      cyw43_arch_lwip_end();
   }

   return success;
}
