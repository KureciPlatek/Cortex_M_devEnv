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

/* --- Callbacks for  */
// static void sub_request_cb(void *arg, err_t err);
// static void unsub_request_cb(void *arg, err_t err);
static void pub_request_cb(__unused void *arg, err_t err);

// static void sub_unsub_topics(MQTT_CLIENT_DATA_T *mqtt_handle, bool sub);
// static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
// static void adc_worker_fn(async_context_t *context, async_at_time_worker_t *worker);
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
// static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);

/**
 * @brief   Measure values of ADC to send random data to MQTT broker
 * @param   none
 * @return  [float] value of ADC
 */
float mqtt_read_adc(void)
{
   /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
   const float conversionFactor = 3.3f / (1 << 12);

   float adc = (float)adc_read() * conversionFactor;
   float value = 27.0f - (adc - 0.706f) / 0.001721f;

   return value;
}

// /**
//  * @brief Add mqtt client name to topic or not. If not, only subtopic name will be sent
//  *        Set MQTT_FULL_TOPIC to concatenate client_id before name (which is sub/topic/like)
//  * @param mqtt_handle   MQTT client handle
//  * @param name          Sub topic name to be published or on which we subscribed
//  * @return              concantenated name (or not)
//  */
// const char *full_topic(MQTT_CLIENT_DATA_T *mqtt_handle, const char *name)
// {
// #if MQTT_FULL_TOPIC
//    static char full_topic[MQTT_TOPIC_LEN];
//    snprintf(full_topic, sizeof(full_topic), "/%s%s", mqtt_handle->mqtt_client_info.client_id, name);
//    return full_topic;
// #else
//    return name;
// #endif
// }

// /**
//  * @brief LED on or off
//  *
//  * @param mqtt_handle [in/out] MQTT client mqtt_handle
//  * @param on    [in] Activate or not
//  */
// void control_led(MQTT_CLIENT_DATA_T *mqtt_handle, bool on)
// {
//    // Publish mqtt_handle on /state topic and on/off led board
//    const char *message = on ? "On" : "Off";
//    if (on) {
//       cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
//    }
//    else {
//       cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
//    }
// 
//    mqtt_publish(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/led"),
//                 message, strlen(message), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN,
//                 pub_request_cb, mqtt_handle);
// }

void publish_adc(MQTT_CLIENT_DATA_T *mqtt_handle)
{
   float adc_val = mqtt_read_adc();
   // Publish adc_val on /mqtt_device_name/adc_val topic
   char adc_val_string[16];
   snprintf(adc_val_string, sizeof(adc_val_string), "%.2f", adc_val);

   cyw43_arch_lwip_begin();  // Start lwIP critical section
   err_t err = mqtt_publish(mqtt_handle->mqtt_client_inst, MQTT_WILL_TOPIC,
                            adc_val_string, strlen(adc_val_string), 
                            MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN,
                            pub_request_cb, mqtt_handle);
   cyw43_arch_lwip_end();    // End lwIP critical section
   
   if (ERR_OK != err)
   {
      printf("MQTT publish failed: %d\n", err);
   }
   else
   {
      printf("Published: %s to %s\n", adc_val_string, MQTT_WILL_TOPIC);
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
   if (err != 0)
   {
      printf("pub_request_cb failed %d", err);
   }
}

// /**
//  * @brief Subscribe to a topic
//  *
//  * @param arg ?
//  * @param err ?
//  */
// static void sub_request_cb(void *arg, err_t err)
// {
//    MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
//    if (err != 0)
//    {
//       printf("ERRO subscribe request failed %d", err);
//    }
//    mqtt_handle->subscribe_count++;
// }

// /**
//  * @brief Unsubscribe to a topic
//  *
//  * @param arg ?
//  * @param err ?
//  */
// static void unsub_request_cb(void *arg, err_t err)
// {
//    MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
//    if (err != 0)
//    {
//       printf("ERRO unsubscribe request failed %d", err);
//    }
//    mqtt_handle->subscribe_count--;
//    assert(mqtt_handle->subscribe_count >= 0);
// 
//    // Stop if requested
//    if (mqtt_handle->subscribe_count <= 0 && mqtt_handle->stop_client)
//    {
//       mqtt_disconnect(mqtt_handle->mqtt_client_inst);
//    }
// }


// /**
//  * @brief subscribe or unsubscribe to topics
//  *
//  * @param mqtt_handle handle to MQTT communication
//  * @param sub         [bool] true: subscribe to all topics, false, unsubscribe
//  */
// static void sub_unsub_topics(MQTT_CLIENT_DATA_T *mqtt_handle, bool sub)
// {
//    mqtt_request_cb_t callbckFnc = sub ? sub_request_cb : unsub_request_cb;
//    mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/led"),   MQTT_SUBSCRIBE_QOS, callbckFnc, mqtt_handle, sub);
//    mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/print"), MQTT_SUBSCRIBE_QOS, callbckFnc, mqtt_handle, sub);
//    mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/ping"),  MQTT_SUBSCRIBE_QOS, callbckFnc, mqtt_handle, sub);
//    mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/exit"),  MQTT_SUBSCRIBE_QOS, callbckFnc, mqtt_handle, sub);
// }


// /**
//  * @brief
//  * @param arg
//  * @param data
//  * @param len
//  * @param flags
//  */
// static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
// {
//    MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
// #if MQTT_FULL_TOPIC
//    const char *basic_topic = mqtt_handle->topic + strlen(mqtt_handle->mqtt_client_info.client_id) + 1;
// #else
//    const char *basic_topic = mqtt_handle->topic;
// #endif
//    strncpy(mqtt_handle->data, (const char *)data, len);
//    mqtt_handle->len = len;
//    mqtt_handle->data[len] = '\0';
// 
//    printf("Topic: %s, Message: %s\n", mqtt_handle->topic, mqtt_handle->data);
//    if (strcmp(basic_topic, "/led") == 0)
//    {
//       if (lwip_stricmp((const char *)mqtt_handle->data, "On") == 0 || strcmp((const char *)mqtt_handle->data, "1") == 0)
//          control_led(mqtt_handle, true);
//       else if (lwip_stricmp((const char *)mqtt_handle->data, "Off") == 0 || strcmp((const char *)mqtt_handle->data, "0") == 0)
//          control_led(mqtt_handle, false);
//    }
//    else if (strcmp(basic_topic, "/print") == 0)
//    {
//       printf("%.*s\n", len, data);
//    }
//    else if (strcmp(basic_topic, "/ping") == 0)
//    {
//       char buf[11];
//       snprintf(buf, sizeof(buf), "%lu", to_ms_since_boot(get_absolute_time()) / 1000);
//       mqtt_publish(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/uptime"), buf, strlen(buf), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, mqtt_handle);
//    }
//    else if (strcmp(basic_topic, "/exit") == 0)
//    {
//       mqtt_handle->stop_client = true;      // stop the client when ALL subscriptions are stopped
//       sub_unsub_topics(mqtt_handle, false); // unsubscribe
//    }
// }


// /**
//  * @brief
//  * @param arg
//  * @param topic
//  * @param tot_len
//  */
// static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
// {
//    MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
//    strncpy(mqtt_handle->topic, topic, sizeof(mqtt_handle->topic));
// }


// /**
//  * @brief
//  * @param context
//  * @param worker
//  */
// static void adc_worker_fn(async_context_t *context, async_at_time_worker_t *worker)
// {
//    MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)worker->user_data;
//    publish_adc(mqtt_handle);
//    async_context_add_at_time_worker_in_ms(context, worker, TEMP_WORKER_TIME_S * 1000);
// }


/**
 * @brief
 * @param client
 * @param arg
 * @param status
 */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
   printf("[INFO] mqtt conn cb\n");
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
   if (status == MQTT_CONNECT_ACCEPTED)
   {
      mqtt_handle->connect_done = true;
//      sub_unsub_topics(mqtt_handle, true); // subscribe;

      // indicate online
      if (mqtt_handle->mqtt_client_info.will_topic)
      {
         mqtt_publish(mqtt_handle->mqtt_client_inst, mqtt_handle->mqtt_client_info.will_topic, "1", 1, MQTT_WILL_QOS, true, pub_request_cb, mqtt_handle);
      }

      // Publish temperature every 10 sec if it's changed
//      _adc_worker.user_data = mqtt_handle;
//      async_context_add_at_time_worker_in_ms(cyw43_arch_async_context(), &_adc_worker, 0);
   }
   else if (status == MQTT_CONNECT_DISCONNECTED)
   {
      mqtt_handle->connect_done = false;
//      if (!mqtt_handle->connect_done)
//      {
//         printf("ERRO Failed to connect to mqtt server");
//      }
   }
   else
   {
      printf("ERRO Unexpected status");
   }
}

/**
 * @brief start client
 *
 * @param mqtt_handle
 */
bool mqtt_start_client(MQTT_CLIENT_DATA_T *mqttComm_handle)
{
   const int port = MQTT_PORT;
   bool success = false;

   /* Create new mqtt client */
   mqttComm_handle->mqtt_client_inst = mqtt_client_new();

   if (!mqttComm_handle->mqtt_client_inst)
   {
      printf("MQTT client instance creation error\n");
      success = false;
   }
   else
   {
      success = true;
      printf("IP address of this device %s\n", ipaddr_ntoa(&(netif_list->ip_addr)));
      printf("Connecting to mqtt server at %s\n", ipaddr_ntoa(&mqttComm_handle->mqtt_server_address));
   }

   if(success)
   {
      /* As lwIP API is not thread safe (function mqtt_client_connect()), we have to acquire a lock, prior to call it */
      cyw43_arch_lwip_begin();
      printf("Good\n");
      if (ERR_OK != mqtt_client_connect(mqttComm_handle->mqtt_client_inst, &mqttComm_handle->mqtt_server_address, port, mqtt_connection_cb, mqttComm_handle, &mqttComm_handle->mqtt_client_info))
      {
         printf("MQTT broker connection error\n");
         success = false;
      }
      printf("[INFO] connected\n");
//      mqtt_set_inpub_callback(mqttComm_handle->mqtt_client_inst, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, mqttComm_handle);
      /* Release lwIP stack lock */
      cyw43_arch_lwip_end();
   }

   return success;
}
