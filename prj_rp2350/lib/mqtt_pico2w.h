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

#include "board.h"

#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/dns.h"
#include "lwip/altcp_tls.h"

#define MQTT_SERVER           "10.43.0.117"
#define MQTT_SERVER_PORT      1883  /* Often this port */
#define MQTT_TOPIC_LEN        100
#define MQTT_KEEP_ALIVE_S     60    /* Seconds */
// qos passed to mqtt_subscribe
#define MQTT_SUBSCRIBE_QOS    1  /* At most once (QoS 0) */
#define MQTT_PUBLISH_QOS      1  /* At least once (QoS 1) */
#define MQTT_PUBLISH_RETAIN   0  /* Exactly once (QoS 2) */
// #define MQTT_FULL_TOPIC       /* Activate to add client ID to topic name like MQTT_DEVICE_NAME/sub/topic/name */

// topic used for last will and testament
#define MQTT_WILL_TOPIC       "/online"
#define MQTT_WILL_MSG         "0"
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
 * MAC:	                  90-65-84-E8-20-CE
 */
#define WIFI_SSID             "NoserWlanPortal" /* For Noser only */
#define WIFI_PASSWORD         ""
#define WIFI_CONN_TIMEOUT     30000 /* in millisec */

/* How often measure temperature */
#define TEMP_WORKER_TIME_S 10

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
void dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg);


#endif /* MQTT_PICO2W_H */