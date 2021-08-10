/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define TEST_RX 				0
#define TEST_TX 				0
#define TEST_RESPONSE		1
#define TEST_MQTT 			0
#define TEST_HTTP 			1
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

UART_Emul_HandleTypeDef UartEmulHandle;

response_t g_check_new_msg = RESPONSE_ERR;
uint8_t g_recv_byte = 0, g_count = 0, g_count_temp = 0;  
uint8_t g_isDone = RX_FALSE, g_check_end = 1;
uint8_t g_recv_buff[MAX_SIZE_BUFF] = {0};
uint8_t g_buff_temp[MAX_SIZE_BUFF] = {0};
uint8_t g_buffer_log[30] = {0};
uint8_t g_id_msg = 0, g_size_sms = 0, g_size_IP = 0;
uint32_t g_timeNow = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */
  
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	Log_Info((uint8_t*)"Inited\n", 7);
	HAL_Delay(500);
	Enable_LTE();
	HAL_Delay(500);
	Blynk();
  
	/*Wait util init success*/
	HAL_Delay(15000);
	Blynk();
	Log_Info((uint8_t*)"Done", 4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

#if TEST_RESPONSE == 1
	HAL_Delay(1000);
	Trans_Data(&UartEmulHandle, (uint8_t*)"AT+GMI\r", 7);
	if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
	{
			Log_Info((uint8_t*)"RES_OK\n", 7);
	}
	else Log_Info((uint8_t*)"RES_ERR\n", 8);
	
	/*Check Baudrate of EC200*/
//	Check_Baud_LTE();
//	/*Check sim detect of EC200 and enable*/
//	Enable_SIM();
//	/*Check CPIN of module LTE*/
//	Check_CPIN_LTE();
//	/*Select Text mode for SMS*/
//	Select_Text_Mode();
//	/*Select ME Memory store sms*/
//	Select_ME_Memory();
//	/*Delete ME Memory store sms*/
//	Delete_Memory_SMS();
#if TEST_MQTT == 1
	MQTT_Recv_Mode(0, 0, 1);
	MQTT_Session(0, 0);
	
	if(MQTT_Open(0, (uint8_t*)"www.maqiatto.com", 1883) == RESPONSE_OK) 
	{
			MQTT_Connect(0, 0, (uint8_t*)"qn052289@gmail.com", (uint8_t*)"182739");
			MQTT_Publish(0, 0, 0, 1, (uint8_t*)"qn052289@gmail.com/RGB_Blue", 5, (uint8_t*)"12345", 1);
	}
#endif

#if TEST_HTTP == 1
	HAL_Delay(3000);
	HTTP_Config_ID(1);
//		HTTP_Enable_Request(1);
	HTTP_Enable_Response(1);
	HTTP_Config_TCP_IP(1, 1, (uint8_t*)"m3-world",(uint8_t*)"mms", (uint8_t*)"mms", 1);
	HTTP_Deactive_PDP(1);
	HTTP_Active_PDP(1);
	HTTP_Set_URL(25, 80, (uint8_t*)"https://demo.traccar.org/");
	HTTP_Get_Range((uint8_t*)"20", (uint8_t*)"35", 8, 1);
	
	g_size_IP = HTTP_Ping_IP(1, (uint8_t*)"demo.traccar.org", g_buff_temp);
	if(g_size_IP != 0) Log_Info(g_buff_temp, g_size_IP);
#endif

#endif

  while (1)
  {	
#if TEST_RESPONSE == 1
		
		if(Recv_Response_Continue(&UartEmulHandle, 300) == RESPONSE_NEW_MSG) //if receive a notice for new sms
		{
				Log_Info((uint8_t*)"RES_MSG\n", 8);	
				memset(g_buff_temp, '\0', MAX_SIZE_BUFF);
				g_id_msg = Get_ID_Msg(g_recv_buff);	//get id of message from response
				sprintf((char*)g_buff_temp, "0AT+CMGR=%d\r", g_id_msg);
				Trans_Data(&UartEmulHandle, g_buff_temp, 11);	//send read AT command
				if(Recv_Response_Continue(&UartEmulHandle, 500) == RESPONSE_OK)	//get response
				{						
						Log_Info((uint8_t*)"RES_OK\n", 7);	
						g_size_sms = Get_Data_Msg(g_recv_buff, g_buff_temp, g_count_temp);
				}
				if(Compare_Str(g_buff_temp, (uint8_t*)"http", g_size_sms))
				{
						Log_Info((uint8_t*)"\nCorrect\n", 9);
				}
				Delete_Memory_SMS();
		}
//		Blynk();
#endif

#if TEST_RX == 1
		
			Trans_Data(&UartEmulHandle, (uint8_t*)"0ATI\r", 5);
			while(!g_isDone)
			{		
					g_timeNow = HAL_GetTick();
					Recv_Data(&UartEmulHandle, g_timeNow, 350);
			}
			g_check_end = 1;
			Log_Info(g_recv_buff, g_count);
			g_isDone = RX_FALSE;
			g_count = 0;
			memset(g_recv_buff, '0', MAX_SIZE_BUFF);
			HAL_Delay(700);
#endif

#if TEST_TX == 1		
			memset(g_recv_buff, '0', 30);
			HAL_Delay(500);
			Trans_Data(&UartEmulHandle, (uint8_t*)"AT+GMI\r", 7);
			if (HAL_UART_Emul_Receive_DMA(&UartEmulHandle, g_recv_buff, 24) != HAL_OK)
			{
					HAL_Delay(500);
					Log_Info((uint8_t*)"Error_Handler\n", 14);
			}
			while (__HAL_UART_EMUL_GET_FLAG(&UartEmulHandle, UART_EMUL_FLAG_RC) != SET){};
			__HAL_UART_EMUL_CLEAR_FLAG(&UartEmulHandle, UART_EMUL_FLAG_RC);
			Log_Info((uint8_t*)g_recv_buff, 24);
				
			memset(g_recv_buff, '0', 30);
			HAL_Delay(500);			
			Trans_Data(&UartEmulHandle, (uint8_t*)"AT+CGMM\r", 8);
			if (HAL_UART_Emul_Receive_DMA(&UartEmulHandle, g_recv_buff, 24) != HAL_OK)
			{
					HAL_Delay(500);
					Log_Info((uint8_t*)"Error_Handler\n", 14);
			}
			while (__HAL_UART_EMUL_GET_FLAG(&UartEmulHandle, UART_EMUL_FLAG_RC) != SET){};
			__HAL_UART_EMUL_CLEAR_FLAG(&UartEmulHandle, UART_EMUL_FLAG_RC);
			Log_Info((uint8_t*)g_recv_buff, 24);
			HAL_Delay(500);
#endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7|GPIO_PIN_8|PWR_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, WAKEUP_CTRL_Pin|RESET_CTRL_Pin|PWRKEY_CTRL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PC7 PC8 PWR_EN_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : WAKEUP_CTRL_Pin PWRKEY_CTRL_Pin */
  GPIO_InitStruct.Pin = WAKEUP_CTRL_Pin|PWRKEY_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RESET_CTRL_Pin */
  GPIO_InitStruct.Pin = RESET_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RESET_CTRL_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
//  __disable_irq();
  while (1)
  {
			HAL_Delay(500);
//			Log_Info((uint8_t*)"Error_Handler\n", 14);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/