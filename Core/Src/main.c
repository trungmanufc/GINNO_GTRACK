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


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TIME_5MINUTE 60000
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;



/* USER CODE BEGIN PV */

UART_HandleTypeDef uartGPS;
UART_HandleTypeDef uartLog;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
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
uint32_t u32CurrentTime = 0;
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
	HAL_UART_Transmit(&uartLog, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

	return ch;
}





char* testBuffer2 = "$GNRMC,152657.000,A,2057.82025,N,10549.33270,E,0.00,0.00,060821,,,A,V*04\r\n$GNGGA,152658.000,2057.82021,N,10549.33274,E,1,12,1.0,46.7,M,-27.8,M,,*59\r\n";

/* The Rx Buffer from the Quectel L76 LB */
char rxBuffer[1500];

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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  u8Test = SC7A20_Init();


  //char cGpsOnly[100] = "$PMTK353,1,0,0,0,0*2B\n\r";
  //HAL_UART_Transmit_IT(&uartGPS, (uint8_t*)cGpsOnly, strlen(cGpsOnly));

  /*Enable GPS*/
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  //Quectel_Init();
  HAL_UART_Receive_IT(&uartGPS, (uint8_t*)rxBuffer, sizeof(rxBuffer));

  /* Print log to indicate that we initialize the program */
  printf("************GTRACK STM32 PROGRAM*************\r\n");

  /*TEST GPS PARSE*/


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
	  if ((HAL_GetTick() - u32CurrentTime) > TIME_5MINUTE)
	  {
		  printf("!!!INMOTION DETECTION!!!\r\n");
	  }

	  SC7A20_coordinate_read(&testXYZ);
	  u8ReceiveTest = SC7A20_read(SC7A20_ADDR_INT1_SOURCE);
	  //HAL_UART_Receive(&uartLog, (uint8_t*)rxBuffer, (sizeof(rxBuffer) - 1), HAL_MAX_DELAY);

	  if(bIsMotion)
	  {
		  printf("!!!!MOTION DETECTED !!!!!\n\r");
		  bIsMotion = false;
	  }

	  /* Print log the Buffer Received from the Quectel L76 */
	  //printf("%s\n\r",rxBuffer);

	  /*printf("******ACCELERATION in 3 AXIS*******\n\r");
	  printf("Acceleration X: %d\n\r", testXYZ.u16XCoor);
	  printf("Acceleration Y: %d\n\r", testXYZ.u16YCoor);
	  printf("Acceleration Z: %d\n\r", testXYZ.u16ZCoor);
	  printf("**********END OF RECEPTION*********\n\n\n\r");
*/
	  /* Print LOG start the while loop */
	  printf("\r\n****START THE CONVERSION******\r\n\n");



	  char test_GNGGA[100] = {0};
	  char test_GNRMC[100] = {0};

	   /* 2 strings to split the GNGAA from the NMEA sent from the Quectel L76 LB */
		printf("%s\r\n\n", rxBuffer);
		printf("%d\r\n\n", strlen(rxBuffer));
		gps_read(testBuffer2, &test_L76, test_GNGGA, test_GNRMC);

		HAL_Delay(500);
	  //BLINK LED
	  /*HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7 | GPIO_PIN_8);
	  HAL_Delay(1000);
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7 | GPIO_PIN_8);
	  HAL_Delay(1000);*/
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  uartLog.Instance = USART1;
  uartLog.Init.BaudRate = 115200;
  uartLog.Init.WordLength = UART_WORDLENGTH_8B;
  uartLog.Init.StopBits = UART_STOPBITS_1;
  uartLog.Init.Parity = UART_PARITY_NONE;
  uartLog.Init.Mode = UART_MODE_TX_RX;
  uartLog.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartLog.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&uartLog) != HAL_OK)
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
  uartGPS.Instance = USART2;
  uartGPS.Init.BaudRate = 9600;
  uartGPS.Init.WordLength = UART_WORDLENGTH_8B;
  uartGPS.Init.StopBits = UART_STOPBITS_1;
  uartGPS.Init.Parity = UART_PARITY_NONE;
  uartGPS.Init.Mode = UART_MODE_TX_RX;
  uartGPS.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartGPS.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&uartGPS) != HAL_OK)
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC7 PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
	u8StatusInt1++;
	u32CurrentTime = HAL_GetTick();
	if ( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
	{
		bIsMotion = true;
	}

	else if ( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
	{
		bIsInMotion = true;
	}
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  //__NOP();
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
  HAL_UART_Receive_IT(&uartGPS, (uint8_t*)rxBuffer, sizeof(rxBuffer));
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
