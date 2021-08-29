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
#include"SC7A20.h"
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include"L76.h"
#include"w25q16.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TIME_5MINUTE 300000
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEST_MQTT		0
#define TEST_MQTT_SSL	0
#define TEST_FLASH 		0
#define MAX_SIZE_BUFF	256
#define TEST_GPS		1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
uint8_t u8SC7A20Error = RESET;
uint8_t u8I2cTest = RESET;
uint8_t u8Test = SC7A20_ADDR_CTRL_REG1;
uint8_t u8ReceiveTest = RESET;
uint8_t u8Data = 0;
Coordinate_t testXYZ;
uint8_t isMotion = RESET;
//uint8_t isInMotion = RESET;
uint8_t u8StatusInt1 = 0;
bool bIsMotion = false;
bool bIsInMotion = false;
bool bIsSetGPS = false;
bool bIsStop = false;
uint32_t u32CurrentTime = 0;


/**
  * @brief  enum type for GPS
  * @retval None
  */


/* Quang's global variables */
uint8_t 		g_buffer_log[30] = {0};
uint8_t 		g_write_buffer[MAX_SIZE_BUFF] = {0};
uint8_t		 	g_read_buffer[MAX_SIZE_BUFF] = {0};
uint8_t 		g_data_write = 0x89;
uint8_t 		g_data_read = 0;
GPS_t   		g_gps_data = {0};


UART_Emul_HandleTypeDef UartEmulHandle;

response_t 		g_flag = RESPONSE_ERR;

//global variables for LTE module
uint8_t    		g_recv_byte = 0, g_count = 0, g_count_temp = 0;
uint8_t 		g_isDone = RX_FALSE, g_check_end = 1;
uint8_t 		g_recv_buff[MAX_SIZE_BUFF] = {0};
uint8_t 		g_buff_temp[MAX_SIZE_BUFF] = {0};
uint8_t 		g_id_msg = 0, g_size_sms = 0, g_size_IP = 0;
uint8_t 		g_mqtt_isOn = OFF;
uint32_t 		g_timeNow = 0;
char 			g_buff_send[30] = {0};

static void softUART_DeInit(void);
static void softUART_ReInit(void);
static void LTE_Disable(void);
static void FlashMQTT_WriteRead(void);
static void Wakeup_CallBack(void);
static void LTE_Init(void);
static void Flash_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNU__C */

PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

	return ch;
}





char* testBuffer2 = "$GNRMC,152657.000,A,2057.82025,N,10549.33270,E,0.00,0.00,060821,,,A,V*04\r\n$GNGGA,152658.000,2057.82021,N,10549.33274,E,1,12,1.0,46.7,M,-27.8,M,,*59\r\n";

/* The Rx Buffer from the Quectel L76 LB */
char rxBuffer[1000];

char *testBuffer = "$GNRMC,573843,A,3349.896,N,11808.521,W,000.0,360.0,230108,013.4,E*69\r\n$GNGGA,185833.80,4808.7402397,S,01133.9325039,W,1,15,1.1,470.50,M,45.65,M,,*75\r\n";
/* Strings to parse the GPGGA NMEA */

char *sL76Time = '\0';
char *sL76Lat = '\0';
/* 2 strings to split the GNGAA from the NMEA sent from the Quectel L76 LB */
//char *sSubGPS = '\0';
char *sGNGGA ;
L76 test_L76;
char* testComma = "GNGGA,185833.80,4808.7402397,N,01133.9325039,E,0,15,1.1,470.50,M,45.65,M,,*75";
uint8_t testIndexofComma[20];

/* MQTT Open Contact */
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
	  char test_GNGGA[100] = {0};
	  char test_GNRMC[100] = {0};
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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  /* Acce init */
  u8Test = SC7A20_Init();

  /* Flash memory init */
  Flash_Init();

  HAL_Delay(500);

  /* LTE module Init */
  LTE_Init();

  	/**************MQTT TEST ***************/
  	/***************************************/
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

			if(MQTT_Check_Connect() != RESPONSE_OK)
		{
				MQTT_Open_Connect();
		}
		HAL_Delay(1000);
		/*Test Pub data*/
		MQTT_Publish(0, 0, 0, 1, (uint8_t*)"qn052289@gmail.com/topic1", 5, (uint8_t*)"12345");
	#endif

	HAL_UART_Receive_IT(&huart2, (uint8_t*)rxBuffer, sizeof(rxBuffer));

	/* Print log to indicate that we initialize the program */
	printf("************GTRACK STM32 PROGRAM*************\r\n");

	/*TEST GPS PARSE*/

	#if TEST_FLASH == 1

		/* Test Write and Read a byte*/
		Log_Info((uint8_t*)"Write & Read\r\n", 14);
		W25Q16_WriteByte(g_data_write, 0x000001);
		W25Q16_ReadByte(&g_data_read, 0x000001);
		HAL_Delay(500);

		for (uint8_t i = 0; i < 10; i++)
		{
			g_write_buffer[i] = i + 0x30;
		}

		HAL_Delay(500);
		Log_Info((uint8_t*)"Write Page\r\n", 12);
		W25Q16_WritePage(g_write_buffer, 3, 0x03, 10);

		HAL_Delay(500);
		Log_Info((uint8_t*)"ReadSomeByte\r\n", 14);
		W25Q16_ReadSomeBytes(g_read_buffer, 0x000303, 10);

		HAL_Delay(500);

		for (uint8_t i = 0; i < 10; i++)
		{
				sprintf((char*)g_buffer_log, "%02x\r\n", g_read_buffer[i]);
				Log_Info((uint8_t*)g_buffer_log, 5);
		}
	#endif



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	#if (TEST_GPS == 1)

	  /* Check whether the inmotion state lasts 5 minutes or not */
	  if (((HAL_GetTick() - u32CurrentTime) > TIME_5MINUTE) && (bIsSetGPS == true))
	  {
		  bIsInMotion = true;
		  printf("!!!INMOTION DETECTION!!!\r\n");
		  bIsSetGPS = false;
		  bIsMotion = false;
	  }

	  SC7A20_coordinate_read(&testXYZ);							/* Debug purpose */
	  u8ReceiveTest = SC7A20_read(SC7A20_ADDR_INT1_SOURCE);  	/* Debug purpose */

	  /* Motion detected after stop mode */
	  if(bIsMotion && bIsStop)
	  {
		  Wakeup_CallBack();
	  }

	  /* If the motion is detected, the GPS will get data and then publish the data to MQTT server */
	  if(bIsSetGPS)
	  {
		  /* Ensure that the power of GPS module is turned on */
		  gps_power_EnOrDi(ENABLE);

		  printf("\r\n****START THE CONVERSION******\r\n\n");

		  /* 2 strings to split the GNGAA from the NMEA sent from the Quectel L76 LB */
		  printf("%s\r\n\n", rxBuffer);
		  printf("%d\r\n\n", strlen(rxBuffer));
		  gps_read(rxBuffer, &test_L76, test_GNGGA, test_GNRMC);

		  /* Write to Flash, Read from Flash and then publish the infos to MQTT serser */
		  FlashMQTT_WriteRead();
	  }
	  else					/* InMotion status */
	  {
		  printf("MOTION NOT DETECTED YET !!\r\n");

		  /* Disable GPS Module */
		  gps_power_EnOrDi(DISABLE);

		  /* DeInit the RX pin of UART */
		  softUART_DeInit();

		  /* LTE POWER DISABLE */
		  LTE_Disable();

		  /* Enter sleep mode */
		  printf("Enter Stop Mode:\r\n");

		  bIsStop = true;

		  HAL_SuspendTick();

		  HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
	  }

	#endif

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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|WAKEUP_CTRL_Pin|RESET_CTRL_Pin|PWRKEY_CTRL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|WAKEUP_CTRL_Pin|PWRKEY_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC7 PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : RESET_CTRL_Pin */
  GPIO_InitStruct.Pin = RESET_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RESET_CTRL_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */

static void LTE_Disable(void)
{
	HAL_GPIO_WritePin(PWR_EN_PORT, PWR_EN_PIN, GPIO_PIN_SET);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  HAL_UART_Receive_IT(&huart2, (uint8_t*)rxBuffer, sizeof(rxBuffer));
}

static void softUART_DeInit(void)
{
	HAL_GPIO_DeInit(UART_EMUL_RX_PORT, UART_EMUL_RX_PIN);
}


static void softUART_ReInit(void)
{
	GPIO_InitTypeDef   GPIO_InitStruct;

	/* Configure GPIOC for UART Emulation Rx */
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pin  = UART_EMUL_RX_PIN;

	HAL_GPIO_Init(UART_EMUL_RX_PORT, &GPIO_InitStruct);

	/*##-2- Enable NVIC for line Rx  #################################*/
	/* Enable and set EXTI Line Interrupt to the highest priority */
	HAL_NVIC_SetPriority(UART_EMUL_EXTI_IRQ, 0, 0);
	HAL_NVIC_EnableIRQ(UART_EMUL_EXTI_IRQ);
}

static void FlashMQTT_WriteRead(void)
{
	g_gps_data.long_t.dLongRaw = test_L76.dLongtitude;
	g_gps_data.lat_t.dLatRaw = test_L76.dLattitude;

	printf("\n\n\rWrite data\r\n");

	for (uint8_t i = 0; i < 8; i++)
	{
			g_write_buffer[i] = g_gps_data.long_t.longBytes[i];
	}
	for (uint8_t i = 8; i < 16; i++)
	{
			g_write_buffer[i] = g_gps_data.lat_t.latBytes[i - 8];
	}

	W25Q16_WritePage(g_write_buffer, 0, 0x00, 16);

	HAL_Delay(500);

	printf("\n\rRead some bytes \r\n");

	W25Q16_ReadSomeBytes(g_read_buffer, 0x00, 16);

	g_gps_data.long_t.dLongRaw = 0;
	g_gps_data.lat_t.dLatRaw = 0;

	for(uint8_t i = 0; i < 8; i++)
	{
		g_gps_data.long_t.longBytes[i] = g_read_buffer[i];
	}

	ftoa(g_gps_data.long_t.dLongRaw, g_buff_send, 6);

	MQTT_Publish(0, 0, 0, 1, (uint8_t*)"qn052289@gmail.com/topic1", strlen(g_buff_send), (uint8_t*)g_buff_send);

	printf("Long: %lf\r\n", g_gps_data.long_t.dLongRaw);

	for (uint8_t i = 0; i < 8; i++)
	{
		g_gps_data.lat_t.latBytes[i] = g_read_buffer[i+8];
	}

	ftoa(g_gps_data.lat_t.dLatRaw, g_buff_send, 6);

	MQTT_Publish(0, 0, 0, 1, (uint8_t*)"qn052289@gmail.com/topic1", strlen(g_buff_send), (uint8_t*)g_buff_send);

	printf("Lat: %lf\r\n", g_gps_data.lat_t.dLatRaw);

	HAL_Delay(500);
}

static void Wakeup_CallBack(void)
{
	  SystemClock_Config();

	  printf("Wake up from sleep mode\r\n");

	  /* GPS enable */
	  gps_power_EnOrDi(ENABLE);

	  /* LTE RX pin enable */
	  softUART_ReInit();

	  /* LTE enable */
	  Enable_LTE();

	  HAL_Delay(15000);

	  MQTT_Recv_Mode(0, 0, 1);
	  MQTT_Session(0, 0);



	  /*Connect with SSL*/
	  MQTT_SSL_Mode(0, 1, 0);
	  MQTT_SSL_Certificate(0);
	  MQTT_SSL_Level(0, 0);
	  MQTT_SSL_Version(0, 4);
	  MQTT_SSL_Ciphersuite(0, (uint8_t*)"0xFFFF");
	  MQTT_SSL_Ignore(0, 1);


//	  /* Check whether the mqtt server is connected or not. If not, reconnected! */
//	  if(MQTT_Check_Connect() != RESPONSE_OK)
//	  {
//			MQTT_Open_Connect();
//			HAL_Delay(500);
//	  }

	  /* Print log */
	  printf("!!!!MOTION DETECTED !!!!!\n\r");

	  /* Re Init SOFTUART */
	  HAL_ResumeTick();

	  /* Quectel initialization */
	  Quectel_Init();

	  /* Re Init GPS UART */
	  HAL_UART_Receive_IT(&huart2, (uint8_t*)rxBuffer, sizeof(rxBuffer));

	  /* Change 2 flags */
	  bIsMotion = false;
	  bIsSetGPS = true;
	  bIsStop = false;
}

static void LTE_Init(void)
{
	memset(&test_L76, 0, sizeof(test_L76));

	Enable_LTE();
	HAL_Delay(15000);
	printf("LTE Enabled!!\r\n");

	HAL_Delay(1000);
	Trans_Data(&UartEmulHandle, (uint8_t*)"AT+GMI\r", 7);

	if (Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK)
	{
		Log_Info((uint8_t*)"RES_OK\n", 7);
	}
	else
	{
		  Log_Info((uint8_t*)"RES_ERR\n", 8);
	}

	/*Check Baudrate of EC200*/
	Check_Baud_LTE();

	/*Check sim detect of EC200 and enable*/
	Enable_SIM();

	/*Check CPIN of module LTE*/
	Check_CPIN_LTE();

	/*Select Text mode for SMS*/
	Select_Text_Mode();

	/*Select ME Memory store sms*/
	Select_ME_Memory();

	/*Delete ME Memory store sms*/
	Delete_Memory_SMS();
}

static void Flash_Init(void)
{
	Log_Info((uint8_t*)"Inited Flash\n", 13);
	W25Q16_Init();
	W25Q16_Erase_Chip();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
