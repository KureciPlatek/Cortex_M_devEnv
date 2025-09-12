/**
 * @file    main.c
 * @author  Jeremie Gallee (jeremie.gallee@noser.com)
 * @brief   main file for RP2350 project example with ThreadX,
 *          gdb/OpenOCD debugging and CMake project management
 * @version 0.1
 * @date    2025-07-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "board.h"
#include "mqtt_pico2w.h"
#include "pico/multicore.h"

static MQTT_CLIENT_DATA_T mqttComm_handle;

int main(void)
{
   stdio_init_all();

   /* GPIO 26, 27 and 28 as ADC0, 1 and 2
    * Use pin 33 as AGND and pin 35 as ADC_VREF (set 3V3(out) to it )*/
   adc_init();

   adc_gpio_init(ADC_PIN_XAXIS);
   adc_gpio_init(ADC_PIN_YAXIS);

   adc_set_temp_sensor_enabled(true);
   printf("\n --- Start program ---\n");

   /* Prepare MQTT client connection */
   mqttComm_handle.mqtt_client_info.client_id    = MQTT_DEVICE_NAME;  // client_id_buf;
   mqttComm_handle.mqtt_client_info.keep_alive   = MQTT_KEEP_ALIVE_S; // Keep alive in sec
#if defined(MQTT_USERNAME) && defined(MQTT_PASSWORD)
   mqttComm_handle.mqtt_client_info.client_user  = MQTT_USERNAME;
   mqttComm_handle.mqtt_client_info.client_pass  = MQTT_PASSWORD;
#else
#error "No password and name provided for MQTT"
#endif

   mqttComm_handle.mqtt_client_info.will_topic   = MQTT_WILL_TOPIC; // will_topic;
   mqttComm_handle.mqtt_client_info.will_msg     = MQTT_WILL_MSG;
   mqttComm_handle.mqtt_client_info.will_qos     = MQTT_WILL_QOS;
   mqttComm_handle.mqtt_client_info.will_retain  = true;

#if LWIP_ALTCP && LWIP_ALTCP_TLS
   // TLS enabled
// #ifdef MQTT_CERT_INC
//    static const uint8_t ca_cert[] = TLS_ROOT_CERT;
//    static const uint8_t client_key[] = TLS_CLIENT_KEY;
//    static const uint8_t client_cert[] = TLS_CLIENT_CERT;
//    // This confirms the indentity of the server and the client
//    mqttComm_handle.mqtt_client_info.tls_config = altcp_tls_create_config_client_2wayauth(ca_cert, sizeof(ca_cert),
//                                                                                client_key, sizeof(client_key), NULL, 0, client_cert, sizeof(client_cert));
// #if ALTCP_MBEDTLS_AUTHMODE != MBEDTLS_SSL_VERIFY_REQUIRED
//    printf("Warning: tls without verification is insecure\n");
// #endif
// #else
   mqttComm_handle->client_info.tls_config = altcp_tls_create_config_client(NULL, 0);
   printf("Warning: tls without a certificate is insecure\n");
//#endif
#endif

   /* Provide IP address to MQTT client */
   ip_addr_t mqttBrokerIP;
   if (!ipaddr_aton(MQTT_SERVER, &mqttBrokerIP))
   {
      printf("Invalid MQTT server address: %s\n", MQTT_SERVER);
      return false;
   }
   mqttComm_handle.mqtt_server_address = mqttBrokerIP;

   /* Start RTOS */
   freeRtos_init(&mqttComm_handle);

   while(true) {}

   printf("mqtt client exiting\n");
   return 0;
}
