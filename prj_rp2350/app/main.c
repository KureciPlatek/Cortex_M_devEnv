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
// #include "pico/unique_id.h"   /* For pico_get_unique_board_id_string */
#include "pico/multicore.h"

static MQTT_CLIENT_DATA_T mqttComm_handle;

int main(void)
{
   stdio_init_all();

   adc_init();
   adc_set_temp_sensor_enabled(true);
   adc_select_input(4);
   printf("\n --- Start program ---\n");

   /* Get pico board unique ID and create a client name out of it*/
//   char unique_id_buf[5];
//   pico_get_unique_board_id_string(unique_id_buf, sizeof(unique_id_buf));
//   for (int i = 0; i < sizeof(unique_id_buf) - 1; i++)
//   {
//      unique_id_buf[i] = tolower(unique_id_buf[i]);
//   }
//
//   char client_id_buf[sizeof(MQTT_DEVICE_NAME) + sizeof(unique_id_buf) - 1];
//   memcpy(&client_id_buf[0], MQTT_DEVICE_NAME, sizeof(MQTT_DEVICE_NAME) - 1);
//   memcpy(&client_id_buf[sizeof(MQTT_DEVICE_NAME) - 1], unique_id_buf, sizeof(unique_id_buf) - 1);
//   client_id_buf[sizeof(client_id_buf) - 1] = 0;
//   printf("Device name %s\n", client_id_buf);

   /* Prepare MQTT client connection */
   mqttComm_handle.mqtt_client_info.client_id    = MQTT_DEVICE_NAME;  // client_id_buf;
   mqttComm_handle.mqtt_client_info.keep_alive   = MQTT_KEEP_ALIVE_S; // Keep alive in sec
#if defined(MQTT_USERNAME) && defined(MQTT_PASSWORD)
   mqttComm_handle.mqtt_client_info.client_user  = MQTT_USERNAME;
   mqttComm_handle.mqtt_client_info.client_pass  = MQTT_PASSWORD;
#else
#error "No password and name provided for MQTT"
#endif

//   static char will_topic[MQTT_TOPIC_LEN];
//   strncpy(will_topic, full_topic(&mqttComm_handle, MQTT_WILL_TOPIC), sizeof(will_topic));
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
