/**
 * @file    mqtt_pico2w.h
 * @author  Jeremie Gallee (jeremie.gallee@noser.com)
 * @brief   MQTT prototype to use with a PICO2W board (RP2350)
 *          Uses pico-sdk
 * @version 0.1
 * @date    2025-08-07
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef MQTT_PICO2W_H
#define MQTT_PICO2W_H

/* Use of Asynchrone context of FreeRTOS
 *(could be _poll.h or _threadsafe_background.h)
 */
#include "pico/async_context_freertos.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/dns.h"
#include "board.h"
#if LWIP_ALTCP && LWIP_ALTCP_TLS
#include "lwip/altcp_tls.h"
#endif

#define MQTT_SERVER           "10.42.0.1" // RPi5 wifi hotspot
/* IP Adress of RPi5 WiFi AP (hotspot): 10.42.0.1 = 0A.2A.00.01 (hexa) */
#define MQTT_SERVER_HEXA      ((u32_t)0xA2A0001UL)
#define MQTT_SERVER_PORT      1883
#define MQTT_TOPIC_LEN        100
#define MQTT_KEEP_ALIVE_S     60    /* Seconds */
// qos passed to mqtt_subscribe
#define MQTT_SUBSCRIBE_QOS    1  /* At most once (QoS 0) */
#define MQTT_PUBLISH_QOS      1  /* At least once (QoS 1) */
#define MQTT_PUBLISH_RETAIN   0  /* Exactly once (QoS 2) */

/* Topic description */
#define MQTT_WILL_TOPIC       "rpi5_topic"
#define MQTT_WILL_MSG         "Hello from Pico W!"
#define MQTT_WILL_QOS         1
#define MQTT_DEVICE_NAME      "pico2w_jerem"

#define MQTT_LED_TOPIC        "/led"
#define MQTT_LED_CMD_ON       "On"
#define MQTT_LED_CMD_OFF      "Off"
#define MQTT_PRINT_TOPIC      "/print"
#define MQTT_PRINT_UNSUBLED   "unsub_led"
#define MQTT_PRINT_SUB_LED    "sub_led"

/* @todo move to CMake or compile time */
#define MQTT_USERNAME         "RPI5_JEREM"
#define MQTT_PASSWORD         "m05qu1770"
/* Wi-Fi config:
 * 
 * sudo nmcli device wifi hotspot ssid IoT_HS password 107_H5_4242
 * Device 'wlan0' successfully activated with '72989d1b-e195-49e0-b7ae-c2f58329611f'.
 * Hint: "nmcli dev wifi show-password" shows the Wi-Fi name and password
 * 
 * Send a message to subscriber:
 * $~: mosquitto_pub -d -t rpi5_topic -m "Prout" -u RPI5_JEREM -P m05qu1770 -h 10.42.0.1 
 * 
 * Receive a message:
 * $~: mosquitto_sub -d -t rpi5_topic_2 -u RPI5_JEREM -P m05qu1770 -h 10.42.0.1
 */
#define WIFI_SSID             "rpi5_hotspot" /* For Noser only */
#define WIFI_PASSWORD         "rp15_h075p07" /* No password on GuestWLAN */
#define WIFI_CONN_TIMEOUT     30000 /* in millisec */

/**
 * @brief MQTT client handle.
 * It will be used by the project as main and unique handle
 */
typedef struct {
   mqtt_client_t *mqtt_client_inst;
   struct mqtt_connect_client_info_t mqtt_client_info;
   char data[MQTT_OUTPUT_RINGBUF_SIZE];
   char topic[MQTT_TOPIC_LEN];
   uint16_t len;
   ip_addr_t mqtt_server_address;
   bool connect_done;
   int  subscribe_count;
} MQTT_CLIENT_DATA_T;


float mqtt_read_adc(void);
void publish_temperature(MQTT_CLIENT_DATA_T *mqtt_handle);
void publish_gyro(MQTT_CLIENT_DATA_T *mqtt_handle, float angSpeed_X, float angSpeed_Y);
bool mqtt_start_client(MQTT_CLIENT_DATA_T *mqtt_handle);

#endif /* MQTT_PICO2W_H */
