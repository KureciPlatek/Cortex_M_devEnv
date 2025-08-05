/**
 * @file    main.c
 * @author  Jeremie Gallee (jeremie.gallee@noser.com)
 * @brief   main file for RP2040 project example with ThreadX, 
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

int main(void)
{
   stdio_init_all();
   while (true)
   {
      printf("Hello, world!\n");
      sleep_ms(1000);
   }
   return 0;
}
