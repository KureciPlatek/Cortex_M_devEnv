/**
 * @file    app_threadx.h
 * @author  Jeremie Gallee (jeremie.gallee@noser.com)
 * @brief   Demo for ThreadX on RP2350 uC on a pico2 w board
 * @version 0.1
 * @date    2025-08-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __APP_THREADX_H__
#define __APP_THREADX_H__

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"

/* Private defines -----------------------------------------------------------*/
#define USE_STATIC_ALLOCATION       1
#define TX_APP_MEM_POOL_SIZE        3 * 1024

#define TX_APP_SINGLE_MSG_SIZE      1
#define TX_APP_MSG_QUEUE_NB_MSG     10
#define TX_APP_MSG_QUEUE_FULL_SIZE  TX_APP_SINGLE_MSG_SIZE *TX_APP_MSG_QUEUE_NB_MSG
#define TX_APP_STACK_SIZE           512
#define TX_APP_THREAD_PRIO          5
#define TOGGLE_LED                  1

#define RECEIVER_THREAD_PRIO        10
#define RECEIVER_THREAD_PREEMPTION_THRESHOLD RECEIVER_THREAD_PRIO

/* Main thread defines -------------------------------------------------------*/
#ifndef TX_APP_THREAD_PREEMPTION_THRESHOLD
#define TX_APP_THREAD_PREEMPTION_THRESHOLD TX_APP_THREAD_PRIO
#endif

#ifndef TX_APP_THREAD_TIME_SLICE
#define TX_APP_THREAD_TIME_SLICE TX_NO_TIME_SLICE
#endif
#ifndef TX_APP_THREAD_AUTO_START
#define TX_APP_THREAD_AUTO_START TX_AUTO_START
#endif

/* @todo use MUTEX */
#ifdef USE_TX_MUTEX
/* Use TX_MUTEX as sync object */
#define APP_SYNC_TYPE TX_MUTEX
#define TX_SYNC_ERROR TX_MUTEX_ERROR

#define APP_SYNC_GET tx_mutex_get
#define APP_SYNC_PUT tx_mutex_put
#define APP_SYNC_CREATE(a) tx_mutex_create((a), "App Mutex", TX_NO_INHERIT)

#else
/* define TX_SEMAPHORE  as sync object*/
#define APP_SYNC_TYPE      TX_SEMAPHORE
#define TX_SYNC_ERROR      TX_SEMAPHORE_ERROR

#define APP_SYNC_GET       tx_semaphore_get
#define APP_SYNC_PUT(a)    tx_semaphore_ceiling_put((a), 1)
#define APP_SYNC_CREATE(a) tx_semaphore_create((a), "App Binary Semaphore", 1)
#endif

UINT threadx_demo_init(VOID *memory_ptr);
void threadx_demo_start(void);
void MsgSenderThreadOne_Entry(ULONG thread_input);

#ifdef __cplusplus
}
#endif
#endif /* __APP_THREADX_H__ */
