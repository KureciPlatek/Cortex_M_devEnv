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

#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "pico/async_context_freertos.h"  // Use of Asynchrone context of FreeRTOS (could be _poll.h or _threadsafe_background.h)
#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/dns.h"
// #include "lwip/altcp_tls.h"

// #define MQTT_SERVER           "10.43.0.117"
#define MQTT_SERVER           "10.42.0.1" // RPi5 wifi hotspot
// No DNS stuff, go easy with IP addr: 10.43.0.117 = 0A.2B.00.75
// If go on Noser guestWan:   IP addr: 10.43.18.0  = 0A.2B.12.00
//#define MQTT_SERVER_HEXA      ((u32_t)0x0A2B0075UL)
#define MQTT_SERVER_HEXA      ((u32_t)0x0A2B1200UL)
#define MQTT_SERVER_PORT      1883  /* Often this port */
#define MQTT_TOPIC_LEN        100
#define MQTT_KEEP_ALIVE_S     60    /* Seconds */
// qos passed to mqtt_subscribe
#define MQTT_SUBSCRIBE_QOS    1  /* At most once (QoS 0) */
#define MQTT_PUBLISH_QOS      1  /* At least once (QoS 1) */
#define MQTT_PUBLISH_RETAIN   0  /* Exactly once (QoS 2) */
// #define MQTT_FULL_TOPIC       /* Activate to add client ID to topic name like MQTT_DEVICE_NAME/sub/topic/name */

// topic used for last will and testament
#define MQTT_WILL_TOPIC       "rpi5_topic"
#define MQTT_WILL_MSG         "Hello from Pico W!"
#define MQTT_WILL_QOS         1
#define MQTT_DEVICE_NAME      "pico2w_jerem"


/* @todo move to CMake or compile time */
#define MQTT_USERNAME         "RPI5_JEREM"
#define MQTT_PASSWORD         "m05qu1770"
/* Wi-Fi config:
 * SSID:                   NoserWlanPortal
 * Protokoll:              Wi-Fi 5 (802.11ac)
 * Security:               WPA2-Enterprise (encoding: AES)
 * Smartcard or other certificate
 * Canal:                  100
 * IPv6-Adress:            fe80::9acf:4a55:a542:9fa4%24
 * IPv4-Adress:            10.43.12.47
 * IPv4-DNS-Server:        10.0.128.188
 *                         10.0.128.189
 * DNS-Suffix:             nosergroup.lan
 *                         guest.lan
 * MAC:	                  90-65-84-E8-20-CE
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
//#define WIFI_SSID             "NoserWlanPortal" /* For Noser only */
#define WIFI_SSID             "IoT_HS" /* For Noser only */
#define WIFI_PASSWORD         "107_H5_4242" /* No password on GuestWLAN */
#define WIFI_CONN_TIMEOUT     30000 /* in millisec */

/* Asynchrone worker timeout.
 A worker is called only one time. It is like a timer with an interrupt
 Thank you guys to redevelop the wheel...
 */
#define MQTT_ASYNC_WORKER_TIMEOUT 1 /* ms */

#ifdef MQTT_CERT_INC
#include MQTT_CERT_INC
#endif

/**
 * @brief MQTT client handle.
 * It will be used by the project as main and unique handle
 */
typedef struct {
   mqtt_client_t *mqtt_client_inst;
   struct mqtt_connect_client_info_t mqtt_client_info;
   char data[MQTT_OUTPUT_RINGBUF_SIZE];
   char topic[MQTT_TOPIC_LEN];
   uint32_t len;
   ip_addr_t mqtt_server_address;
   bool connect_done;
   int  subscribe_count;
   bool stop_client;
} MQTT_CLIENT_DATA_T;


float mqtt_read_adc(void);
void control_led(MQTT_CLIENT_DATA_T *mqttComm_handle, bool on);
void publish_adc(MQTT_CLIENT_DATA_T *mqttComm_handle);
bool mqtt_start_client(MQTT_CLIENT_DATA_T *mqttComm_handle);

#endif /* MQTT_PICO2W_H */
