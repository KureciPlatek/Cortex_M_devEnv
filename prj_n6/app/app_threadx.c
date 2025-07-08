/**
 ******************************************************************************
 * @file    app_threadx.c
 * @author  MCD Application Team
 * @brief   ThreadX applicative file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
#include "stm32n6xx_hal.h"
#include "board.h"

/* Thread objects */
TX_THREAD tx_app_thread;
TX_THREAD ThreadTwo;
/* semaphore */
APP_SYNC_TYPE SyncObject;

/* Private function prototypes -----------------------------------------------*/
VOID ThreadTwo_Entry(ULONG thread_input);
static VOID Led_Toggle(uint16_t led_pin, GPIO_TypeDef *led_port, UINT iter);
void App_Delay(uint32_t Delay);

/* Byte pool for all threads */
#if (USE_STATIC_ALLOCATION == 1)
__ALIGN_BEGIN static UCHAR tx_byte_pool_buffer[TX_APP_MEM_POOL_SIZE] __ALIGN_END;
static TX_BYTE_POOL tx_app_byte_pool;
#endif

/* Private function prototypes -----------------------------------------------*/
void Error_Handler(void); /* Coming from main */

/**
 * @brief  Define the initial system.
 * @param  first_unused_memory : Pointer to the first unused memory
 * @retval None
 */
VOID tx_application_define(VOID *first_unused_memory)
{
#if (USE_STATIC_ALLOCATION == 1)
   UINT status = TX_SUCCESS;
   VOID *memory_ptr;

   if (tx_byte_pool_create(&tx_app_byte_pool, "Tx App memory pool", tx_byte_pool_buffer, TX_APP_MEM_POOL_SIZE) != TX_SUCCESS)
   {
      Error_Handler();
   }
   else
   {
      memory_ptr = (VOID *)&tx_app_byte_pool;
      status = App_ThreadX_Init(memory_ptr);
      if (status != TX_SUCCESS)
      {
         Error_Handler();
      }
   }

#else
   /* Using dynamic memory allocation requires to apply some changes to the linker file.    */

   (void)first_unused_memory;
#endif
}

/**
 * @brief  Application ThreadX Initialization.
 * @param memory_ptr: memory pointer
 * @retval int
 */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
   UINT ret = TX_SUCCESS;
   TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *)memory_ptr;

   CHAR *pointer;

   /* Allocate the stack for Thread One */
   if (tx_byte_allocate(byte_pool, (VOID **)&pointer, TX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
   {
      return TX_POOL_ERROR;
   }
   /* Create Thread One.  */
   if (tx_thread_create(&tx_app_thread, "Thread One", ThreadOne_Entry, 0, pointer,
                        TX_APP_STACK_SIZE, TX_APP_THREAD_PRIO, TX_APP_THREAD_PREEMPTION_THRESHOLD,
                        TX_APP_THREAD_TIME_SLICE, TX_APP_THREAD_AUTO_START) != TX_SUCCESS)
   {
      return TX_THREAD_ERROR;
   }

   /* Allocate the stack for ThreadTwo.  */
   if (tx_byte_allocate(byte_pool, (VOID **)&pointer, TX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
   {
      ret = TX_POOL_ERROR;
   }

   /* Create ThreadTwo.  */
   if (tx_thread_create(&ThreadTwo, "Thread Two", ThreadTwo_Entry, 0,
                        pointer, TX_APP_STACK_SIZE,
                        TX_APP_THREAD_PRIO, TX_APP_THREAD_PREEMPTION_THRESHOLD,
                        TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
   {
      ret = TX_THREAD_ERROR;
   }

   /* Create the Synchronization API used by ThreadOne and ThreadTwo.*/
   if (APP_SYNC_CREATE(&SyncObject) != TX_SUCCESS)
   {
      ret = TX_SYNC_ERROR;
   }

   return ret;
}
/**
 * @brief  Function implementing the ThreadOne_Entry thread.
 * @param  thread_input: Not used.
 * @retval None
 */
void ThreadOne_Entry(ULONG thread_input)
{
   UNUSED(thread_input);
   ULONG iteration = 0;

   /* Infinite loop */
   while (1)
   {
      /* try to acquire the sync object without waiting */
      if (APP_SYNC_GET(&SyncObject, TX_NO_WAIT) == TX_SUCCESS)
      {
         printf("** ThreadOne : SyncObject acquired ** \n");

         /*sync object acquired, toggle the LED_GREEN each 500ms for 5s */
         Led_Toggle(LED_GREEN_Pin, LED_GREEN_GPIO_Port, 10);

         /*release the sync object */
         APP_SYNC_PUT(&SyncObject);

         printf("** ThreadOne : SyncObject released ** \n");

         tx_thread_sleep(1);
      }
      else
      {

         if ((iteration % 2000000) == 0)
         {
            printf("** ThreadOne : waiting for SyncObject !! **\n");
         }
      }
      iteration++;
   }
}

/**
 * @brief  ThreadX_Init
 * @param  None
 * @retval None
 */
void ThreadX_Init(void)
{
   tx_kernel_enter();
}

/**
 * @brief  Function implementing the ThreadTwo thread.
 * @param  thread_input: Not used
 * @retval None
 */
void ThreadTwo_Entry(ULONG thread_input)
{
   UNUSED(thread_input);
   ULONG iteration = 0;

   /* Infinite loop */
   while (1)
   {
      /* try to acquire the sync object without waiting */
      if (APP_SYNC_GET(&SyncObject, TX_NO_WAIT) == TX_SUCCESS)
      {
         printf("** ThreadTwo : SyncObject acquired ** \n");

         /*Sync object acquired toggle the LED_RED each 500ms for 5s*/
         Led_Toggle(LED_RED_Pin, LED_RED_GPIO_Port, 10);

         /*release the sync object*/
         APP_SYNC_PUT(&SyncObject);

         printf("** ThreadTwo : SyncObject released ** \n");

         tx_thread_sleep(1);
      }
      else
      {
         if ((iteration % 2000000) == 0)
         {
            printf("** ThreadTwo : waiting for SyncObject !! **\n");
         }
      }
      iteration++;
   }
}

/**
 * @brief Critical section function that needs acquiring SyncObject.
 * @param  led: LED to toggle
 * @param  iter: Number of iterations
 * @retval None
 */
static VOID Led_Toggle(uint16_t led_pin, GPIO_TypeDef *led_port, UINT iter)
{
   UINT i;

   HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_SET);

   for (i = 0; i < iter; i++)
   {
      HAL_GPIO_TogglePin(led_port, led_pin);
      /* Toggle the Led each 500ms */
      App_Delay(50);
   }

   HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_SET);
}

/**
 * @brief  Application Delay function.
 * @param  Delay : number of ticks to wait
 * @retval None
 */
void App_Delay(uint32_t Delay)
{
   ULONG initial_time = tx_time_get();
   while ((tx_time_get() - initial_time) < Delay) {}
}
