/**
 ******************************************************************************
 * @file    ThreadX/Tx_Thread_MsgQueue/Src/main.c
 * @author  MCD Application Team
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2020-2021 STMicroelectronics.
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
#include "board.h"

#if defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
// static void configureSWO(void);

#define SWO_BAUDRATE 115200U
#define ARM_LAR_ACCESS_ENABLE 0xC5ACCE55

/**
 * @brief overwrite _write() from syscall.c to use ITM_SendChar() instead of __io_putchar()
 */
// int _write(int file, char *ptr, int len)
// {
//    for (int i = 0; i < len; ++i)
//    {
//       ITM_SendChar(*ptr++);
//    }
//    return len;
// }

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
   /* MPU Configuration--------------------------------------------------------*/
   MPU_Config();

   /* Enable I/D-Cache---------------------------------------------------------*/
   SCB_EnableICache();
   SCB_EnableDCache();

   /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
   HAL_Init();

   /* Configure the system clock */
   SystemClock_Config();

   /* Initialize all configured peripherals */
   MX_GPIO_Init();
   MX_USART3_UART_Init();

// #ifdef USE_SWO
//    configureSWO();
// #endif

   MX_ThreadX_Init();

//   __HAL_DBGMCU_FREEZE_HRTIM

   /* Infinite loop */
   while (1){}
}

// void configureSWO(void)
// {
//    /* RCC and GPIO alternate function done elsewere */
//
//    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Bit 24 of DEMCR Debug Exception and Monitor Control Register
//
// #if (__CORTEX_M == 0x07U)
//    /* in Cortex M7, the trace needs to be unlocked via the DWT->LAR register with 0xC5ACCE55 value */
//    DWT->LAR = ARM_LAR_ACCESS_ENABLE;
// #endif
//
//    const uint32_t clock_frequency = HAL_RCC_GetHCLKFreq(); // rcc_ahb_frequency;
// 	const uint32_t divisor = (clock_frequency / SWO_BAUDRATE) - 1U;
//
//    /* Configure TPI as 1 bit mode? */
//    TPI->SPPR = (0x01 & TPI_SPPR_TXMODE_Msk); /* b10 = UART/NRZ (async) b01 = Manchester (See TPIU_SPPR definition in ARM) */
//    TPI->ACPR = divisor;
//
//
// //   TPIU_CURRENT_PORT_SIZE     = 1; /* port size = 1 bit */
// //   TPIU_SELECTED_PIN_PROTOCOL = 1; /* trace port protocol = Manchester */
// //   TPIU_ASYNC_CLOCK_PRESCALER = (HAL_RCC_GetHCLKFreq() / SWO_FREQ) - 1;
// //   TPIU_FORMATTER_AND_FLUSH_CONTROL = 0x100; /* turn off formatter (0x02 bit) */
//    DBGMCU->CR = DBGMCU_CR_DBG_SLEEPD1 | DBGMCU_CR_DBG_STOPD1 | DBGMCU_CR_DBG_STANDBYD1 | DBGMCU_CR_DBG_TRGOEN;
//
// //   SWO->SPPR;
//
//    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;


//   uint32_t myVar1 = TPI->ACPR;
//   uint32_t myVar2 = DBGMCU->CR;

//   ITM->LAR = 0xC5ACCE55;
//   ITM->TCR = ITM_TCR_TraceBusID_Msk | ITM_TCR_SWOENA_Msk | ITM_TCR_SYNCENA_Msk | ITM_TCR_ITMENA_Msk;
//   ITM->TPR = ITM_TPR_PRIVMASK_Msk; /* all ports accessible unprivileged */
//   ITM->TER = 1; /* enable stimulus channel 0, used with ITM_SendChar() */
//
//   /* this apparently turns off sync packets, see SYNCTAP in DDI0403D pdf: */
//   DWT->CTRL = 0x400003FE;
//   printf("This var: %ld %ld\n", myVar1, myVar2);
// }

/**
 * @brief System Clock Configuration
 * @retval None
 * How to get were comes it all from:
 * __HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSI
 */
void SystemClock_Config(void)
{
   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

   /** Supply configuration update enable  */
   HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

#ifdef USE_STLINK_DEBUGGER
   /** Configure the main internal regulator output voltage  */
   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

   while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

   /** Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure.  */
   /* If uses MCO (Microcontroller Clock Output) of STLink (if HSEState = RCC_HSE_BYPASS), then clock freq on PH0_OSC_IN is 8 MHz */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState       = RCC_HSE_BYPASS;
   RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE; /* PLLSRC on Figure 45 clock source tree. hse_ck in this case */
   RCC_OscInitStruct.PLL.PLLM       = 4;
   RCC_OscInitStruct.PLL.PLLN       = 260;   /* DIVNx */
   RCC_OscInitStruct.PLL.PLLP       = 1;     /* DIVPx (pllx_p_ck) */
   RCC_OscInitStruct.PLL.PLLQ       = 2;     /* DIVQx (pllx_q_ck) */
   RCC_OscInitStruct.PLL.PLLR       = 2;     /* DIVRx (pllx_r_ck) */
   RCC_OscInitStruct.PLL.PLLRGE     = RCC_PLL1VCIRANGE_1;
   RCC_OscInitStruct.PLL.PLLVCOSEL  = RCC_PLL1VCOWIDE;
   RCC_OscInitStruct.PLL.PLLFRACN   = 0;
   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
      Error_Handler();
   }

   /** Initializes the CPU, AHB and APB buses clocks  */
   RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK  | RCC_CLOCKTYPE_PCLK1
                                   | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
   RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV2;
   RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
   RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

#else

 /*
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 520000000 (CPU Clock)
  *            HCLK(Hz)                       = 260000000 (AXI and AHBs Clock)
  *            AHB Prescaler                  = 1
  *            rcc_hclk4 /AHB4                = 260 MHz
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  130MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  130MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  130MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  130MHz)
  *            HSI Frequency(Hz)              = 8000000
  *            PLL_M                          = 4
  *            PLL_N                          = 260
  *            PLL_P                          = 1
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 3
  */

   /** Configure the main internal regulator output voltage  */
   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

   while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

   /* Use HSI if using external debugger (STLink deactivated = no HSE input fro STLink'd MCO) */
   RCC_OscInitStruct.OscillatorType       = RCC_OSCILLATORTYPE_HSI;
   RCC_OscInitStruct.HSIState             = RCC_HSI_DIV8;
   RCC_OscInitStruct.HSICalibrationValue  = 64;
   RCC_OscInitStruct.PLL.PLLState         = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource        = RCC_PLLSOURCE_HSI;

   RCC_OscInitStruct.PLL.PLLM             = 4;
   RCC_OscInitStruct.PLL.PLLN             = 260; /* DIVNx */
   RCC_OscInitStruct.PLL.PLLFRACN         = 0;
   RCC_OscInitStruct.PLL.PLLP             = 1;   /* DIVPx (pllx_p_ck) */
   RCC_OscInitStruct.PLL.PLLQ             = 2;   /* DIVQx (pllx_q_ck) */
   RCC_OscInitStruct.PLL.PLLR             = 2;   /* DIVRx (pllx_r_ck) */

   RCC_OscInitStruct.PLL.PLLVCOSEL        = RCC_PLL1VCOWIDE;
   RCC_OscInitStruct.PLL.PLLRGE           = RCC_PLL1VCIRANGE_1;

   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
      Error_Handler();
   }

   /** Initializes the CPU, AHB and APB buses clocks  */
   RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_HCLK  | RCC_CLOCKTYPE_SYSCLK  | RCC_CLOCKTYPE_PCLK1
                                     | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
   RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.SYSCLKDivider   = RCC_SYSCLK_DIV2;
   RCC_ClkInitStruct.AHBCLKDivider   = RCC_HCLK_DIV1;
   RCC_ClkInitStruct.APB3CLKDivider  = RCC_APB3_DIV2;
   RCC_ClkInitStruct.APB1CLKDivider  = RCC_APB1_DIV2;
   RCC_ClkInitStruct.APB2CLKDivider  = RCC_APB2_DIV2;
   RCC_ClkInitStruct.APB4CLKDivider  = RCC_APB4_DIV2;

#endif

   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
   {
      Error_Handler();
   }
}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void)
{
   huart3.Instance                     = USART3;
   huart3.Init.BaudRate                = 115200;
   huart3.Init.WordLength              = UART_WORDLENGTH_8B;
   huart3.Init.StopBits                = UART_STOPBITS_1;
   huart3.Init.Parity                  = UART_PARITY_NONE;
   huart3.Init.Mode                    = UART_MODE_TX_RX;
   huart3.Init.HwFlowCtl               = UART_HWCONTROL_NONE;
   huart3.Init.OverSampling            = UART_OVERSAMPLING_16;
   huart3.Init.OneBitSampling          = UART_ONE_BIT_SAMPLE_DISABLE;
   huart3.Init.ClockPrescaler          = UART_PRESCALER_DIV1;
   huart3.AdvancedInit.AdvFeatureInit  = UART_ADVFEATURE_NO_INIT;
   if (HAL_UART_Init(&huart3) != HAL_OK)
   {
      Error_Handler();
   }
   if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
   {
      Error_Handler();
   }
   if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
   {
      Error_Handler();
   }
   if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
   {
      Error_Handler();
   }
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   /* GPIO Ports Clock Enable */
   __HAL_RCC_GPIOH_CLK_ENABLE();
   __HAL_RCC_GPIOB_CLK_ENABLE();
   __HAL_RCC_GPIOD_CLK_ENABLE();

   /*Configure GPIO pin Output Level */
   HAL_GPIO_WritePin(GPIOB, LED_GREEN_Pin | LED_RED_Pin, GPIO_PIN_RESET);

   /*Configure GPIO pins : LED_GREEN_Pin LED_RED_Pin */
   GPIO_InitStruct.Pin   = LED_GREEN_Pin | LED_RED_Pin;
   GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull  = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

   // WARNING causes error
//   /* Configure PB3 as AF0 (SWO) */
//   GPIO_InitStruct.Pin       = DBG_SWO_Pin;
//   GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
//   GPIO_InitStruct.Pull      = GPIO_NOPULL;
//   GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
//   GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;  // TRACESWO
//
//   HAL_GPIO_Init(DBG_SWO_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
   HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);
   return ch;
}

/* MPU Configuration */
void MPU_Config(void)
{
   MPU_Region_InitTypeDef MPU_InitStruct = {0};

   /* Disables the MPU */
   HAL_MPU_Disable();

   /** Initializes and configures the Region and the memory to be protected  */
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.Number = MPU_REGION_NUMBER0;
   MPU_InitStruct.BaseAddress = 0x0;
   MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
   MPU_InitStruct.SubRegionDisable = 0x87;
   MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
   MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
   MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
   MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
   MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

   HAL_MPU_ConfigRegion(&MPU_InitStruct);
   /* Enables the MPU */
   HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
   HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
   /* Error message */
   printf("** Error occurred ** \n\r");
   while (1)
   {
      HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
      HAL_Delay(1000);
   }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
   /* Infinite loop */
   while (1)
   {
   }
}
#endif /* USE_FULL_ASSERT */
