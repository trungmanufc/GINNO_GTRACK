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
#define TEST_MQTT 			1
#define TEST_MQTT_SSL		1
#define TEST_HTTP 			0
#define TEST_FLASH 			1
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

UART_Emul_HandleTypeDef UartEmulHandle;
uint8_t g_buffer_log[30] = {0};
response_t g_flag = RESPONSE_ERR;

/*global variables for LTE module*/
uint8_t    	g_recv_byte = 0, g_count = 0, g_count_temp = 0;  
uint8_t 		g_isDone = RX_FALSE, g_check_end = 1;
uint8_t 		g_recv_buff[MAX_SIZE_BUFF] = {0};
uint8_t 		g_buff_temp[MAX_SIZE_BUFF] = {0};
uint8_t 		g_id_msg = 0, g_size_sms = 0, g_size_IP = 0;
uint8_t 		g_mqtt_isOn = OFF;
uint32_t 		g_timeNow = 0;

/*global variables for Flash module*/
uint8_t 		g_write_buffer[MAX_SIZE_BUFF] = {0};
uint8_t		 	g_read_buffer[MAX_SIZE_BUFF] = {0};
uint8_t 		g_data_write = 0x89;
uint8_t 		g_data_read = 0;
GPS_t   		g_gps_data = {0};

//test value of longitude, latitude
double 			g_long = 1234.456789;
double 			g_lat  = 3214.876543;
float* 			g_pFloat = 0;
char 				g_buff_send[30] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
response_t MQTT_Open_Connect(void)
{	
	g_flag = RESPONSE_ERR;
	/*wait to open network port 8883*/
	while(g_flag == RESPONSE_ERR)
	{
			#if TEST_MQTT_SSL == 1
			g_flag = MQTT_Open(0, (uint8_t*)"test.mosquitto.org", 8883);
			#else
			g_flag = MQTT_Open(0, (uint8_t*)"test.mosquitto.org", 1883);	
			#endif	
	}
	g_mqtt_isOn = ON;
		
	/*wait to connect to broker*/
	g_flag = RESPONSE_ERR;
	g_flag = MQTT_Connect(0, (uint8_t*)"quang", (uint8_t*)"qn052289@gmail.com", (uint8_t*)"182739");
	while(g_flag != RESPONSE_OK)
	{
			//Reopen network and reconnect
			#if TEST_MQTT_SSL == 1			
			MQTT_Open(0, (uint8_t*)"test.mosquitto.org", 8883);
			#else
			MQTT_Open(0, (uint8_t*)"test.mosquitto.org", 1883);
			#endif
			g_flag = MQTT_Connect(0, (uint8_t*)"quang", (uint8_t*)"qn052289@gmail.com", (uint8_t*)"182739");
	}
	g_flag = RESPONSE_ERR; 
	return RESPONSE_OK;
}
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
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
	Log_Info((uint8_t*)"Inited Flash\n", 13);
	W25Q16_Init();
	W25Q16_Erase_Chip();
	HAL_Delay(500);
	Blynk();
	
	Log_Info((uint8_t*)"Inited LTE\n", 11);
	HAL_Delay(500);
	Enable_LTE();
	HAL_Delay(500);
	Blynk();
  
	/*Wait util init LTE success*/
	HAL_Delay(15000);
	Blynk();
	Log_Info((uint8_t*)"Done\n", 5);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
#if TEST_FLASH == 1

	/* Test Write and Read a byte*/
	Log_Info((uint8_t*)"Write & Read\n", 13);
	W25Q16_WriteByte(g_data_write, 0x000001);
	W25Q16_ReadByte(&g_data_read, 0x000001);
	HAL_Delay(500);
	for(uint8_t i = 0; i < 10; i++)
	{
			g_write_buffer[i] = i + 0x30;
	}
	HAL_Delay(500);
	Log_Info((uint8_t*)"Write Page\n", 11);
	W25Q16_WritePage(g_write_buffer, 3, 0x03, 10);
	HAL_Delay(500);
	Log_Info((uint8_t*)"ReadSomeByte\n", 13);
	W25Q16_ReadSomeBytes(g_read_buffer, 0x000303, 10);
	HAL_Delay(500);
	for(uint8_t i = 0; i < 10; i++)
	{
			sprintf((char*)g_buffer_log, "%02x\n", g_read_buffer[i]);
			Log_Info((uint8_t*)g_buffer_log, 4);
	}
#endif

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

	#if TEST_MQTT_SSL == 1
	/*Connect with SSL*/
	MQTT_SSL_Mode(0, 1, 0);
	MQTT_SSL_Certificate(0);
	MQTT_SSL_Level(0, 0);
	MQTT_SSL_Version(0, 4);
	MQTT_SSL_Ciphersuite(0, (uint8_t*)"0xFFFF");
	MQTT_SSL_Ignore(0, 1);
		#endif	
//	/*wait to open network port 8883*/
//	while(g_flag == RESPONSE_ERR)
//	{
//			g_flag = MQTT_Open(0, (uint8_t*)"test.mosquitto.org", 8883);
//	}
//	g_mqtt_isOn = ON;
//	
//	#else
//	/*Connect without SSL*/
//	/*wait to open network port 1883*/
//	while(g_flag == RESPONSE_ERR)
//	{
////			g_flag = MQTT_Open(0, (uint8_t*)"www.maqiatto.com", 1883);
//			g_flag = MQTT_Open(0, (uint8_t*)"test.mosquitto.org", 1883);		
//	}
//	g_mqtt_isOn = ON;

//	/*wait to connect to broker*/
//	g_flag = RESPONSE_ERR;
//	g_flag = MQTT_Connect(0, (uint8_t*)"quang", (uint8_t*)"qn052289@gmail.com", (uint8_t*)"182739");
////	if(g_flag == RESPONSE_MQTT_CLOSE)	MQTT_Open(0, (uint8_t*)"test.mosquitto.org", 8883);
//	while(g_flag != RESPONSE_OK)
//	{
//			//Reopen network and reconnect
//			#if TEST_MQTT_SSL == 1			
//			MQTT_Open(0, (uint8_t*)"test.mosquitto.org", 8883);
//			#else
//			MQTT_Open(0, (uint8_t*)"test.mosquitto.org", 1883);
//			#endif
//			g_flag = MQTT_Connect(0, (uint8_t*)"quang", (uint8_t*)"qn052289@gmail.com", (uint8_t*)"182739");
//	}
//	g_flag = RESPONSE_ERR; 
	if(MQTT_Check_Connect() != RESPONSE_OK)
	{
			MQTT_Open_Connect();
	}
	HAL_Delay(1000);
	/*Test Pub data*/
	MQTT_Publish(0, 0, 0, 1, (uint8_t*)"qn052289@gmail.com/topic1", 5, (uint8_t*)"12345");
		
	/*Pub data from flash*/
	HAL_Delay(1000);
	MQTT_Publish(0, 0, 0, 1, (uint8_t*)"qn052289@gmail.com/topic1", 10, g_read_buffer);

	/*Test Write, Read Flash and pub data to Broker using Union*/
	W25Q16_Erase_Sector(0);
	g_gps_data.long_t.dLongRaw = g_long;
	g_gps_data.lat_t.dLatRaw = g_lat;
	
	Log_Info((uint8_t*)"Write Data\n", 11);
	for(uint8_t i = 0; i < 8; i++)
	{
			g_write_buffer[i] = g_gps_data.long_t.longBytes[i];
	}
	for(uint8_t i = 8; i < 16; i++)
	{
			g_write_buffer[i] = g_gps_data.lat_t.latBytes[i - 8];
	}
	W25Q16_WritePage(g_write_buffer, 0, 0x00, 16);
	
	HAL_Delay(500);
	Log_Info((uint8_t*)"ReadSomeByte\n", 13);
	W25Q16_ReadSomeBytes(g_read_buffer, 0x00, 16);
	/*Publish Raw data*/
	MQTT_Publish(0, 0, 0, 1, (uint8_t*)"qn052289@gmail.com/topic1", 16, g_read_buffer);
	
	/*Test send string type to MQTT*/
	HAL_Delay(500);
	g_gps_data.long_t.dLongRaw = 0;
	g_gps_data.lat_t.dLatRaw = 0;
	for(uint8_t i = 0; i < 8; i++)
	{
			g_gps_data.long_t.longBytes[i] = g_read_buffer[i];
	}
	ftoa(g_gps_data.long_t.dLongRaw, g_buff_send, 6);
	MQTT_Publish(0, 0, 0, 1, (uint8_t*)"qn052289@gmail.com/topic1", strlen(g_buff_send), (uint8_t*)g_buff_send);
	
	for(uint8_t i = 0; i < 8; i++)
	{
			g_gps_data.lat_t.latBytes[i] = g_read_buffer[i+8];
	}
	ftoa(g_gps_data.lat_t.dLatRaw, g_buff_send, 6);

	MQTT_Publish(0, 0, 0, 1, (uint8_t*)"qn052289@gmail.com/topic1", strlen(g_buff_send), (uint8_t*)g_buff_send);

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
		g_flag = Recv_Response_Continue(&UartEmulHandle, 300);
		
		if(g_flag == RESPONSE_NEW_MSG) //if receive a notice for new sms
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
//		if(MQTT_Check_Connect() != RESPONSE_OK)
//		{
//				MQTT_Open_Connect();
//		}

		
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
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|WAKEUP_CTRL_Pin|RESET_CTRL_Pin|PWRKEY_CTRL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7|GPIO_PIN_8|PWR_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PB12 WAKEUP_CTRL_Pin PWRKEY_CTRL_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_12|WAKEUP_CTRL_Pin|PWRKEY_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC7 PC8 PWR_EN_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
