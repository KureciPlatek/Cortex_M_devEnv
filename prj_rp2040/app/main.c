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
#include "pico/multicore.h"
#include "app_threadx.h"

int main(void)
{
   stdio_init_all();

   gpio_init(PICO_DEFAULT_LED_PIN);
   gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

   printf("\n --- Start program ---\n");

   threadx_demo_start();

   while(true) {}

   printf("Exit program\n"); /* Should never be reached */
   return 0;
}
