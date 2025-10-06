/**
 * @file    app_threadx.c
 * @author  Jeremie Gallee (jeremie.gallee@noser.com)
 * @brief   Demo of ThreadX on RP2350 uC on a pico2_w board. Two threads, one LED
 * @version 0.1
 * @date    2025-08-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"
#include "board.h"

TX_BYTE_POOL tx_app_byte_pool;
UCHAR tx_byte_pool_buffer[TX_APP_MEM_POOL_SIZE];

/* Private variables ---------------------------------------------------------*/
TX_THREAD tx_app_thread;
TX_QUEUE  tx_app_msg_queue;   
TX_THREAD MsgReceiverThread;
TX_THREAD MsgSenderThreadTwo;
TX_QUEUE  MsgQueueTwo;


/* Private function prototypes -----------------------------------------------*/
void MsgSenderThreadTwo_Entry(ULONG thread_input);
void MsgReceiverThread_Entry(ULONG thread_input);

/**
 * @brief  Application ThreadX Initialization.
 * @param memory_ptr: memory pointer
 * @retval int
 */
UINT threadx_demo_init(VOID *memory_ptr)
{
   UINT ret = TX_SUCCESS;
   TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *)memory_ptr;
   CHAR *pointer;

   /* Allocate the stack for Message Queue Sender Thread One  */
   if (tx_byte_allocate(byte_pool, (VOID **)&pointer, TX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
   {
      return TX_POOL_ERROR;
   }
   /* Create Message Queue Sender Thread One.  */
   if (tx_thread_create(&tx_app_thread, "Message Queue Sender Thread One", MsgSenderThreadOne_Entry, 0, pointer,
                        TX_APP_STACK_SIZE, TX_APP_THREAD_PRIO, TX_APP_THREAD_PREEMPTION_THRESHOLD,
                        TX_APP_THREAD_TIME_SLICE, TX_APP_THREAD_AUTO_START) != TX_SUCCESS)
   {
      return TX_THREAD_ERROR;
   }
   /* Allocate the stack for Message Queue One.  */
   if (tx_byte_allocate(byte_pool, (VOID **)&pointer, TX_APP_MSG_QUEUE_FULL_SIZE * sizeof(ULONG), TX_NO_WAIT) != TX_SUCCESS)
   {
      return TX_POOL_ERROR;
   }
   /* Create Message Queue One.  */
   if (tx_queue_create(&tx_app_msg_queue, "Message Queue One", TX_APP_SINGLE_MSG_SIZE, pointer, TX_APP_MSG_QUEUE_FULL_SIZE * sizeof(ULONG)) != TX_SUCCESS)
   {
      return TX_QUEUE_ERROR;
   }

   /* Allocate the stack for Message Queue Sender Thread Two.  */
   if (tx_byte_allocate(byte_pool, (VOID **)&pointer, TX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
   {
      return TX_POOL_ERROR;
   }

   /* Create Message Queue Sender Thread Two.  */
   if (tx_thread_create(&MsgSenderThreadTwo, "Message Queue Sender Thread Two", MsgSenderThreadTwo_Entry, 0, pointer,
                        TX_APP_STACK_SIZE, TX_APP_THREAD_PRIO, TX_APP_THREAD_PREEMPTION_THRESHOLD,
                        TX_APP_THREAD_TIME_SLICE, TX_APP_THREAD_AUTO_START) != TX_SUCCESS)
   {
      return TX_THREAD_ERROR;
   }

   /* Allocate the stack for Message Queue Receiver Thread.  */
   if (tx_byte_allocate(byte_pool, (VOID **)&pointer, TX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
   {
      return TX_POOL_ERROR;
   }

   /* Create Message Queue Receiver Thread.  */
   if (tx_thread_create(&MsgReceiverThread, "Message Queue Receiver Thread", MsgReceiverThread_Entry, 0, pointer,
                        TX_APP_STACK_SIZE, RECEIVER_THREAD_PRIO, RECEIVER_THREAD_PREEMPTION_THRESHOLD,
                        TX_APP_THREAD_TIME_SLICE, TX_APP_THREAD_AUTO_START) != TX_SUCCESS)
   {
      return TX_THREAD_ERROR;
   }
   /* Allocate the Message Queue Two.  */
   if (tx_byte_allocate(byte_pool, (VOID **)&pointer, TX_APP_MSG_QUEUE_FULL_SIZE * sizeof(ULONG), TX_NO_WAIT) != TX_SUCCESS)
   {
      return TX_POOL_ERROR;
   }

   /* Create the Message Queue Two shared by Message Queue Sender Thread Two and Message Queue Receiver Thread.  */
   if (tx_queue_create(&MsgQueueTwo, "Message Queue Two", TX_APP_SINGLE_MSG_SIZE, pointer, TX_APP_MSG_QUEUE_FULL_SIZE * sizeof(ULONG)) != TX_SUCCESS)
   {
      return TX_QUEUE_ERROR;
   }

   return ret;
}
/**
 * @brief  Function implementing the MsgSenderThreadOne_Entry thread.
 * @param  thread_input: Hardcoded to 0.
 * @retval None
 */
void MsgSenderThreadOne_Entry(ULONG thread_input)
{
   ULONG Msg = TOGGLE_LED;
   (void)thread_input;
   /* Infinite loop */
   while (1)
   {
      /* Send message to Message Queue One.  */
      tx_queue_send(&tx_app_msg_queue, &Msg, TX_WAIT_FOREVER);
      /* Sleep for 200ms */
      tx_thread_sleep(20);
   }
}

/**
 * @brief  Function implementing the MsgSenderThreadTwo thread.
 * @param  thread_input: Not used
 * @retval None
 */
void MsgSenderThreadTwo_Entry(ULONG thread_input)
{
   ULONG Msg = TOGGLE_LED;
   (void)thread_input;
   /* Infinite loop */
   while (1)
   {
      /* Send message to MsgQueueTwo.  */
      tx_queue_send(&MsgQueueTwo, &Msg, TX_WAIT_FOREVER);
      /* Sleep for 500ms */
      tx_thread_sleep(50);
   }
}

/**
 * @brief  Function implementing the MsgReceiverThread thread.
 * @param  thread_input: Not used
 * @retval None
 */
void MsgReceiverThread_Entry(ULONG thread_input)
{
   ULONG RMsg = 0;
   UINT status = 0;
   (void)thread_input;
   /* Infinite loop */
   while (1)
   {
      /* Determine whether a message MsgQueueOne or MsgQueueTwo is available */
      status = tx_queue_receive(&tx_app_msg_queue, &RMsg, TX_NO_WAIT);
      if (status == TX_SUCCESS)
      {
         /* Check Message value */
         if (RMsg == TOGGLE_LED)
         {
            printf("** Message is available on MsgQueueOne: RESET_GREEN_LED**\r\n");
            gpio_put(PICO_DEFAULT_LED_PIN, false);
         }
      }
      else
      {
         status = tx_queue_receive(&MsgQueueTwo, &RMsg, TX_NO_WAIT);
         if (status == TX_SUCCESS)
         {
            /* Check Message value */
            if (RMsg == TOGGLE_LED)
            {
               printf("** Message is available on MsgQueueTwo: SET_GREEN_LED**\r\n");
            gpio_put(PICO_DEFAULT_LED_PIN, true);
            }
         }
      }
   }
}

/**
 * @brief Start ThreadX kernel
 * It will call tx_application_define() defined below,
 * which will 
 *    - Create byte pool with tx_byte_pool_create()
 *    - call threadx_demo_init()
 */
void threadx_demo_start(void)
{
   tx_kernel_enter();
}

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

   /* Create byte pool required by ThreadX threads and mechanismus */
   tx_byte_pool_create(&tx_app_byte_pool, "TX_mem_pool", tx_byte_pool_buffer, TX_APP_MEM_POOL_SIZE);

   /* Create and init all threads with the created memory pool */
   memory_ptr = (VOID *)&tx_app_byte_pool;
   status = threadx_demo_init(memory_ptr);

   /* Error */
   if(TX_SUCCESS != status)
   {
      return;
   }

#else
   /*
    * Using dynamic memory allocation requires to apply some changes to the linker file.
    * The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.
    */

   (void)first_unused_memory;
#endif
}
