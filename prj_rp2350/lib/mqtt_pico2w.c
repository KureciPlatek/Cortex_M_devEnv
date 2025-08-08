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
static void sub_request_cb(void *arg, err_t err);
static void unsub_request_cb(void *arg, err_t err);
static void pub_request_cb(__unused void *arg, err_t err);

static void sub_unsub_topics(MQTT_CLIENT_DATA_T *mqtt_handle, bool sub);
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
static void adc_worker_fn(async_context_t *context, async_at_time_worker_t *worker);
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
static void start_client(MQTT_CLIENT_DATA_T *mqtt_handle);


/**
 * @brief asynchrone worker. Give it the function adc_worker_fn()
 * This worker will be used by mqtt_connection_cb() which will create connection
 * and give the worker when calling async_context_add_at_time_worker_in_ms()
 */
static async_at_time_worker_t _adc_worker = {.do_work = adc_worker_fn};


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

/**
 * @brief Add mqtt client name to topic or not. If not, only subtopic name will be sent
 *        Set MQTT_FULL_TOPIC to concatenate client_id before name (which is sub/topic/like)
 * @param mqtt_handle   MQTT client handle
 * @param name          Sub topic name to be published or on which we subscribed
 * @return              concantenated name (or not)
 */
const char *full_topic(MQTT_CLIENT_DATA_T *mqtt_handle, const char *name)
{
#if MQTT_FULL_TOPIC
   static char full_topic[MQTT_TOPIC_LEN];
   snprintf(full_topic, sizeof(full_topic), "/%s%s", mqtt_handle->mqtt_client_info.client_id, name);
   return full_topic;
#else
   return name;
#endif
}

/**
 * @brief LED on or off
 * 
 * @param mqtt_handle [in/out] MQTT client mqtt_handle
 * @param on    [in] Activate or not
 */
void control_led(MQTT_CLIENT_DATA_T *mqtt_handle, bool on)
{
   // Publish mqtt_handle on /state topic and on/off led board
   const char *message = on ? "On" : "Off";
   if (on) {
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
   }
   else {
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
   }

   mqtt_publish(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/led"), 
                message, strlen(message), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, 
                pub_request_cb, mqtt_handle);
}

void publish_adc(MQTT_CLIENT_DATA_T *mqtt_handle)
{
   static float old_adc_val;
   const char *adc_topic_name = full_topic(mqtt_handle, "/adc_val");
   float adc_val = mqtt_read_adc();
   if (adc_val != old_adc_val)
   {
      old_adc_val = adc_val;
      // Publish adc_val on /mqtt_device_name/adc_val topic
      char adc_val_string[16];
      snprintf(adc_val_string, sizeof(adc_val_string), "%.2f", adc_val);

      printf("INFO Publishing %s to %s\n", adc_val_string, adc_topic_name);
      mqtt_publish(mqtt_handle->mqtt_client_inst, adc_topic_name, 
                   adc_val_string, strlen(adc_val_string), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, 
                   pub_request_cb, mqtt_handle);
   }
}

/**
 * @brief Call back with a DNS result
 * @param hostname 
 * @param ipaddr 
 * @param arg 
 */
void dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
   if (ipaddr)
   {
      mqtt_handle->mqtt_server_address = *ipaddr;
      start_client(mqtt_handle);
   }
   else
   {
      panic("dns request failed");
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

/**
 * @brief Subscribe to a topic
 * 
 * @param arg ?
 * @param err ?
 */
static void sub_request_cb(void *arg, err_t err)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
   if (err != 0)
   {
      printf("ERRO subscribe request failed %d", err);
   }
   mqtt_handle->subscribe_count++;
}

/**
 * @brief Unsubscribe to a topic
 * 
 * @param arg ?
 * @param err ?
 */
static void unsub_request_cb(void *arg, err_t err)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
   if (err != 0)
   {
      printf("ERRO unsubscribe request failed %d", err);
   }
   mqtt_handle->subscribe_count--;
   assert(mqtt_handle->subscribe_count >= 0);

   // Stop if requested
   if (mqtt_handle->subscribe_count <= 0 && mqtt_handle->stop_client)
   {
      mqtt_disconnect(mqtt_handle->mqtt_client_inst);
   }
}


/**
 * @brief subscribe or unsubscribe to topics
 * 
 * @param mqtt_handle handle to MQTT communication
 * @param sub         [bool] true: subscribe to all topics, false, unsubscribe 
 */
static void sub_unsub_topics(MQTT_CLIENT_DATA_T *mqtt_handle, bool sub)
{
   mqtt_request_cb_t callbckFnc = sub ? sub_request_cb : unsub_request_cb;
   mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/led"),   MQTT_SUBSCRIBE_QOS, callbckFnc, mqtt_handle, sub);
   mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/print"), MQTT_SUBSCRIBE_QOS, callbckFnc, mqtt_handle, sub);
   mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/ping"),  MQTT_SUBSCRIBE_QOS, callbckFnc, mqtt_handle, sub);
   mqtt_sub_unsub(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/exit"),  MQTT_SUBSCRIBE_QOS, callbckFnc, mqtt_handle, sub);
}


/**
 * @brief
 * @param arg 
 * @param data 
 * @param len 
 * @param flags 
 */
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
#if MQTT_FULL_TOPIC
   const char *basic_topic = mqtt_handle->topic + strlen(mqtt_handle->mqtt_client_info.client_id) + 1;
#else
   const char *basic_topic = mqtt_handle->topic;
#endif
   strncpy(mqtt_handle->data, (const char *)data, len);
   mqtt_handle->len = len;
   mqtt_handle->data[len] = '\0';

   printf("Topic: %s, Message: %s\n", mqtt_handle->topic, mqtt_handle->data);
   if (strcmp(basic_topic, "/led") == 0)
   {
      if (lwip_stricmp((const char *)mqtt_handle->data, "On") == 0 || strcmp((const char *)mqtt_handle->data, "1") == 0)
         control_led(mqtt_handle, true);
      else if (lwip_stricmp((const char *)mqtt_handle->data, "Off") == 0 || strcmp((const char *)mqtt_handle->data, "0") == 0)
         control_led(mqtt_handle, false);
   }
   else if (strcmp(basic_topic, "/print") == 0)
   {
      printf("%.*s\n", len, data);
   }
   else if (strcmp(basic_topic, "/ping") == 0)
   {
      char buf[11];
      snprintf(buf, sizeof(buf), "%lu", to_ms_since_boot(get_absolute_time()) / 1000);
      mqtt_publish(mqtt_handle->mqtt_client_inst, full_topic(mqtt_handle, "/uptime"), buf, strlen(buf), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, mqtt_handle);
   }
   else if (strcmp(basic_topic, "/exit") == 0)
   {
      mqtt_handle->stop_client = true;      // stop the client when ALL subscriptions are stopped
      sub_unsub_topics(mqtt_handle, false); // unsubscribe
   }
}


/**
 * @brief 
 * @param arg 
 * @param topic 
 * @param tot_len 
 */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
   strncpy(mqtt_handle->topic, topic, sizeof(mqtt_handle->topic));
}


/**
 * @brief 
 * @param context 
 * @param worker 
 */
static void adc_worker_fn(async_context_t *context, async_at_time_worker_t *worker)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)worker->user_data;
   publish_adc(mqtt_handle);
   async_context_add_at_time_worker_in_ms(context, worker, TEMP_WORKER_TIME_S * 1000);
}


/**
 * @brief 
 * @param client 
 * @param arg 
 * @param status 
 */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
   MQTT_CLIENT_DATA_T *mqtt_handle = (MQTT_CLIENT_DATA_T *)arg;
   if (status == MQTT_CONNECT_ACCEPTED)
   {
      mqtt_handle->connect_done = true;
      sub_unsub_topics(mqtt_handle, true); // subscribe;

      // indicate online
      if (mqtt_handle->mqtt_client_info.will_topic)
      {
         mqtt_publish(mqtt_handle->mqtt_client_inst, mqtt_handle->mqtt_client_info.will_topic, "1", 1, MQTT_WILL_QOS, true, pub_request_cb, mqtt_handle);
      }

      // Publish temperature every 10 sec if it's changed
      _adc_worker.user_data = mqtt_handle;
      async_context_add_at_time_worker_in_ms(cyw43_arch_async_context(), &_adc_worker, 0);
   }
   else if (status == MQTT_CONNECT_DISCONNECTED)
   {
      if (!mqtt_handle->connect_done)
      {
         printf("ERRO Failed to connect to mqtt server");
      }
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
static void start_client(MQTT_CLIENT_DATA_T *mqtt_handle)
{
#if LWIP_ALTCP && LWIP_ALTCP_TLS
   const int port = MQTT_TLS_PORT;
   printf("Using TLS\n");
#else
   const int port = MQTT_PORT;
   printf("Warning: Not using TLS\n");
#endif

   mqtt_handle->mqtt_client_inst = mqtt_client_new();
   if (!mqtt_handle->mqtt_client_inst)
   {
      printf("MQTT client instance creation error");
   }
   printf("IP address of this device %s\n", ipaddr_ntoa(&(netif_list->ip_addr)));
   printf("Connecting to mqtt server at %s\n", ipaddr_ntoa(&mqtt_handle->mqtt_server_address));

   cyw43_arch_lwip_begin();
   if (mqtt_client_connect(mqtt_handle->mqtt_client_inst, &mqtt_handle->mqtt_server_address, port, mqtt_connection_cb, mqtt_handle, &mqtt_handle->mqtt_client_info) != ERR_OK)
   {
      printf("MQTT broker connection error");
   }
#if LWIP_ALTCP && LWIP_ALTCP_TLS
   // This is important for MBEDTLS_SSL_SERVER_NAME_INDICATION
   mbedtls_ssl_set_hostname(altcp_tls_context(mqtt_handle->mqtt_client_inst->conn), MQTT_SERVER);
#endif
   mqtt_set_inpub_callback(mqtt_handle->mqtt_client_inst, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, mqtt_handle);
   cyw43_arch_lwip_end();
}

