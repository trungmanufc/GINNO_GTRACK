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
typedef struct HexStruct	//a hex format on a line in file .hex
{
	__IO uint8_t byteCount;//1byte
	__IO uint16_t addRess;	//2byte address
	__IO uint8_t recordType; //1byte 
	__IO uint32_t u32data[4];	//4*4 = 16byte
	__IO uint8_t checkSum; //1byte
} HexTypedef_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/*Macro for Bootloader*/
#define APP_ADDRESS 		(uint32_t) 0x08020000
#define APP_SECTOR			FLASH_SECTOR_5

/*Macro for Test*/
#define TEST_RX 				0
#define TEST_TX 				0
#define TEST_RESPONSE		1
#define TEST_MQTT 			1
#define TEST_MQTT_SSL		1
#define TEST_HTTP 			1
#define TEST_FLASH 			1
#define	TEST_WRITE			0
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
uint8_t    	g_recv_byte = 0;
uint16_t 	  g_count = 0, g_count_temp = 0; 
uint8_t 		g_isDone = RX_FALSE, g_check_end = 1;
uint8_t 		g_recv_buff[MAX_SIZE_BUFF] = {0};
uint8_t 		g_buff_temp[MAX_SIZE_BUFF] = {0};
uint8_t 		g_id_msg = 0, g_size_sms = 0, g_size_IP = 0;
uint8_t 		g_mqtt_isOn = OFF;
uint32_t 		g_timeNow = 0;
uint32_t 		g_len_data_http = 0, g_len_read_http = 0;

/*global variables for Flash module*/
uint8_t 		g_data_read = 0;

/*global variables for Bootloader*/
uint32_t 		g_user_address = APP_ADDRESS;
uint8_t 		g_buff_hex_line[50] = {0};
uint32_t 		g_buff_data_boot[10000] = {0};
uint32_t 		g_count_data_boot = 0;
uint32_t 		g_start_line_addr = 0;
HexTypedef_t aLineHex;

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

/**
  * @brief  Get a line in hex file, copy to buffer
	* @param  targetBuff:  buff target to pass data
	* @param  addrStart: address of start data in external flash
  * @retval address of new start data in external flash(stop data + 1)
  */
uint32_t Get_Hex_Line(uint8_t* targetBuff, uint32_t addrStart)
{
		uint32_t newStart = addrStart;
		g_data_read = 'x';
		while(g_data_read != '\n')	//end line
		{
				W25Q16_ReadByte(&g_data_read, newStart);
				targetBuff[newStart - addrStart] = g_data_read;	//copy data to target buffer
				newStart++;
		}
		return newStart;		
}

/**
  * @brief  Convert ascii to decimal
	* @param  character:  character input
  * @retval value in decimal
  */
uint8_t Ascii_to_Dec(uint8_t character)
{
		if(('0' <= character) && (character <= '9')) return (character - '0');
		if(('A' <= character) && (character <= 'F')) return (character - 'A' + 10);
		else return (character - 'a' + 10); //if(('a' <= character) && (character <= 'f')) 
}

/**
  * @brief  Write a Word into internal flash
  * @retval none
  */
void Write_Flash(void)
{
		Log_Info((uint8_t*)"\r\nStartFlash\r\n", 14);
		HAL_FLASH_Unlock();
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
							FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	//erase sector of app
		FLASH_Erase_Sector(APP_SECTOR, FLASH_VOLTAGE_RANGE_3);
	
		for(uint32_t i = 0; i < g_count_data_boot; i++)
		{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, g_user_address, g_buff_data_boot[i]);
				g_user_address += 4;
		}
		HAL_FLASH_Lock();
		Log_Info((uint8_t*)"\r\nEndFlash\r\n", 12);
}
/**
  * @brief  Process a line in hex file
	* @param  line_hex_buff:  buff include data of a hex line
	* @param  addrStart: address of start data in external flash
  * @retval 0(check sum false or not flash data) or 1(check sum true)
  */
uint8_t Process_Hex_Line(uint8_t* line_hex_buff)
{

		uint8_t numberOfData = 0;
		uint16_t check = 0;
		/*
			character 0 is ':', ignore
			character 1,2 are byte count
			charecter 3,4,5,6 are address
			charecter 7,8 are record type
			2 last charecters are check sum
			the rest is data
		*/
	
		//convert byte count, address, record type, check sum  => to DEC
		aLineHex.byteCount = Ascii_to_Dec(line_hex_buff[1]) * 16 + Ascii_to_Dec(line_hex_buff[2]);
		aLineHex.addRess = Ascii_to_Dec(line_hex_buff[3]) * pow(16, 3) + Ascii_to_Dec(line_hex_buff[4]) * pow(16, 2)
											 + Ascii_to_Dec(line_hex_buff[5]) * 16 + Ascii_to_Dec(line_hex_buff[6]);
		aLineHex.recordType = Ascii_to_Dec(line_hex_buff[7]) * 16 + Ascii_to_Dec(line_hex_buff[8]);
		numberOfData = 2 * aLineHex.byteCount;
		aLineHex.checkSum = Ascii_to_Dec(line_hex_buff[9+numberOfData]) * 16 + Ascii_to_Dec(line_hex_buff[10+numberOfData]);
		if((numberOfData > 0) && (aLineHex.recordType == 0x00))
		{
				uint8_t temp_data[32] = {0};	
				for(uint8_t i = 0; i < numberOfData; i++)
				{
					//data from 9,10...,40 on a line
					temp_data[i] = Ascii_to_Dec(line_hex_buff[9+i]);
				}
			
				//convert data => u32bit, eg: 0,1,2,3,4,5,6,7 => 67452301
				for(uint8_t i = 0; i < 4; i++)
				{
					aLineHex.u32data[i] = 0;	
					aLineHex.u32data[i] = ((temp_data[8*i+0] << 4) | temp_data[8*i+1]) << 0; 
					aLineHex.u32data[i] |= ((temp_data[8*i+2] << 4) | temp_data[8*i+3]) << 8;
					aLineHex.u32data[i] |= ((temp_data[8*i+4] << 4) | temp_data[8*i+5]) << 16;
					aLineHex.u32data[i] |= ((temp_data[8*i+6] << 4) | temp_data[8*i+7]) << 24;
				}			
				//check the checksum
				check = aLineHex.byteCount + aLineHex.recordType + (0xFF &(aLineHex.addRess >> 8)) + (0xFF & aLineHex.addRess);
				for(uint8_t i = 0; i < 4; i++)
				{
						for(uint8_t j = 0; j < 4; j++)					
						{
								check += ( 0xFF & (aLineHex.u32data[i] >> (8*j)) );
						}
				}
				if( (0xFF & ((~check) + 1)) == aLineHex.checkSum) 
				{
						Log_Info((uint8_t*)"CheckSum True!\r\n", 16);
						for(uint8_t i = 0; i < 4; i++)
						{
								g_buff_data_boot[g_count_data_boot] = aLineHex.u32data[i];
								g_count_data_boot++;
						}
						return 1;
				}
				else 
				{
						Log_Info((uint8_t*)"CheckSum False\r\n", 16);
						return 0;
				}
		}		
		Log_Info((uint8_t*)"Data not to Flash\r\n", 19);
		return 0; //not flash data
}

void Boot(void)
{
  HAL_RCC_DeInit();
	HAL_DeInit();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;

	/* Clear Pending Interrupt Request, turn  off System Tick*/
	SCB->SHCSR &= ~( SCB_SHCSR_USGFAULTENA_Msk |\
									SCB_SHCSR_BUSFAULTENA_Msk |\
									SCB_SHCSR_MEMFAULTENA_Msk ) ;
	
	__disable_irq ();

	/* Set Main Stack Pointer*/
	__set_MSP(*((volatile uint32_t*) APP_ADDRESS));
	/* Set Program Counter to Blink LED Apptication Address*/
	void (*jump)(void) =  (void(*)(void)) (*((volatile uint32_t *) (APP_ADDRESS + 4)));
  jump();
	while(1);

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint16_t number_of_downloads = 0, len_of_last = 0;
	uint32_t index_of_last = 0;
	uint8_t buff_index[10] = {0};
	uint8_t buff_len[10] = {0};
	uint8_t number_of_write = 0;
	uint16_t id_page_write = 0;
	uint8_t rest_data_last = 0;

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

	
	Log_Info((uint8_t*)"Inited LTE\n", 11);
	HAL_Delay(500);
	Blynk();
	HAL_Delay(500);
	Enable_LTE();
  
	/*Wait util init LTE success*/
	HAL_Delay(15000);
	Blynk();
	Log_Info((uint8_t*)"Done\n", 5);
	/*Change default baudrate of LTE module*/
//	Set_Baud_LTE_4800();
//	Log_Info((uint8_t*)"Changed Baud\n", 13);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	W25Q16_Init();
	W25Q16_Erase_Chip();
	
#if TEST_RESPONSE == 1
	HAL_Delay(1000);
	Trans_Data(&UartEmulHandle, (uint8_t*)"AT+GMI\r", 7);
	if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
	{
			Log_Info((uint8_t*)"RES_OK\n", 7);
	}
	else Log_Info((uint8_t*)"RES_ERR\n", 8);
	
//	/*Check Baudrate of EC200*/
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
	

	#if TEST_HTTP == 1
	HAL_Delay(3000);
	HTTP_Config_ID(1);
//		HTTP_Enable_Request(1);
	HTTP_Enable_Response(1);
	HTTP_Config_TCP_IP(1, 1, (uint8_t*)"m3-world",(uint8_t*)"mms", (uint8_t*)"mms", 1);
	HTTP_Deactive_PDP(1);
	HTTP_Active_PDP(1);
	HTTP_Set_URL(76, 80, (uint8_t*)"https://raw.githubusercontent.com/NHQuang58/Test-Merge/master/Blynk_F401.hex");
	g_len_data_http = HTTP_Get(80);
	
	number_of_downloads = g_len_data_http / 4096;
	len_of_last = g_len_data_http % 4096;
	number_of_write = 4096/256;
	
	//download 4096bytes/time
	for(uint16_t i = 0; i < number_of_downloads; i++)
	{
			//index of download
			memset(buff_index, '\0', 10);
			u16_to_String((char*)buff_index, i*4096);
			HTTP_Get_Range((uint8_t*)"20", buff_index, (uint8_t*)"4096");
			g_len_read_http = HTTP_Read(80, 4096);
			Log_Info((uint8_t*)"Downloaded!\r\n", 13);
			//write to external flash
			if(g_len_read_http == 4096)
			{
					for(uint8_t i = 0; i < number_of_write; i++)
					{
							W25Q16_WritePage(&g_buff_temp[256*i], id_page_write, 0x00, 256);
							id_page_write++;
					}
					Log_Info((uint8_t*)"Write!\r\n", 8);
			}
			else Log_Info((uint8_t*)"Download not enough!\r\n", 22);	
	}
	
	//download rest data in the last time
	if(len_of_last > 0)
	{
			//index of last download
			index_of_last = number_of_downloads * 4096;
			memset(buff_index, '\0', 10);
			u16_to_String((char*)buff_index, index_of_last);
			//len of last download
			memset(buff_len, '\0', 10);
			u16_to_String((char*)buff_len, len_of_last);
			HTTP_Get_Range((uint8_t*)"20", buff_index, buff_len);
			g_len_read_http = HTTP_Read(80, len_of_last);
			Log_Info((uint8_t*)"Downloaded!\r\n", 13);
		
			//write to external flash
			number_of_write = len_of_last/256;
			if(g_len_read_http == len_of_last)
			{
					for(uint8_t i = 0; i < number_of_write; i++)
					{
							W25Q16_WritePage(&g_buff_temp[256*i], id_page_write, 0x00, 256);
							id_page_write++;
					}
					rest_data_last = len_of_last % 256;
					if(rest_data_last > 0)
					{
							for(uint8_t i = 0; i < rest_data_last; i++)
							{
									W25Q16_WriteByte(g_buff_temp[number_of_write * 256 + i], (id_page_write*256) + i);
							}
					}
					Log_Info((uint8_t*)"Write last!\r\n", 13);
			}
			else Log_Info((uint8_t*)"Download last not enough!\r\n", 27);				
	}
	
	#if TEST_WRITE == 1
	W25Q16_ReadByte(&g_data_read, 0x0);
	W25Q16_ReadByte(&g_data_read, 0x0FFF);
	
	W25Q16_ReadByte(&g_data_read, 0x1000);
	W25Q16_ReadByte(&g_data_read, 0x1FFF);
	
	W25Q16_ReadByte(&g_data_read, 0x2000);
	W25Q16_ReadByte(&g_data_read, 0x23C0);
	W25Q16_ReadByte(&g_data_read, 0x23C1);
	W25Q16_ReadByte(&g_data_read, 0x23C2);
	W25Q16_ReadByte(&g_data_read, 0x23C3);
	#endif
//	
//	g_size_IP = HTTP_Ping_IP(1, (uint8_t*)"demo.traccar.org", g_buff_temp);
//	if(g_size_IP != 0) Log_Info(g_buff_temp, g_size_IP);
	#endif

#endif
	
	
	while(g_start_line_addr < g_len_data_http)
	{
			memset(g_buff_hex_line, '\0', 50);
			g_start_line_addr = Get_Hex_Line(g_buff_hex_line, g_start_line_addr);
			Process_Hex_Line(g_buff_hex_line);	
	}
	Write_Flash();
	
	Boot();
  while (1)
  {	

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
