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
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "app_threadx.h"

int main(void)
{
   stdio_init_all();
   if (cyw43_arch_init())
   {
      printf("CYW43 Wi-Fi chip init failed\n");
      return -1;
   }

   while (true)
   {
      printf("Hello, world!\n");
//      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
//      sleep_ms(250);
//      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
//      sleep_ms(250);
      /* Create, init and start ThreadX for this project */
      threadx_demo_start();
   }
   return 0;
}
