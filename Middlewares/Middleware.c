/*
 * Middleware.c
 *
 *  Created on: Sep 28, 2021
 *      Author: Admin
 */


#include "Middleware.h"

/* MQTT Open Contact */
response_t MQTT_Open_Connect(void)
{
	g_flag = RESPONSE_ERR;
	/* Wait to open network port 8883 */
	while(g_flag == RESPONSE_ERR)
	{
			#if TEST_MQTT_SSL == 1
			g_flag = MQTT_Open(0, (uint8_t*)"test.mosquitto.org", 8883);
			#else
			g_flag = MQTT_Open(0, (uint8_t*)"test.mosquitto.org", 1883);
			#endif
	}
	g_mqtt_isOn = ON;

	/* Wait to connect to broker*/
	g_flag = RESPONSE_ERR;
	g_flag = MQTT_Connect(0, (uint8_t*)"quang", (uint8_t*)"qn052289@gmail.com", (uint8_t*)"182739");
	while(g_flag != RESPONSE_OK)
	{
			/* Reopen network and reconnect */
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

static void softUART_DeInit(void); //
static void softUART_ReInit(void); //
static void LTE_Disable(void); //
static void MQTT_Config(void); //
static void Flash_Init(void);
static void Convert_U8_To_Str(char* res, uint8_t inputValue);
static void Write_Read_Pub(void);
extern void SystemClock_Config(void);

static void LTE_Disable(void)
{
	/* Set the pin to disable the the LTE power */
	HAL_GPIO_WritePin(PWR_EN_PORT, PWR_EN_PIN, GPIO_PIN_SET);
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


static void MQTT_Config(void)
{
	/* Config the MQTT */
	MQTT_Recv_Mode(0, 0, 1);
	MQTT_Session(0, 0);

	/*Connect with SSL*/
	MQTT_SSL_Mode(0, 1, 0);
	MQTT_SSL_Certificate(0);
	MQTT_SSL_Level(0, 0);
	MQTT_SSL_Version(0, 4);
	MQTT_SSL_Ciphersuite(0, (uint8_t*)"0xFFFF");
	MQTT_SSL_Ignore(0, 1);
}

static void Flash_Init(void)
{
	Log_Info((uint8_t*)"Inited Flash\n", 13);
	W25Q16_Init();
	W25Q16_Erase_Chip();
}

static void Convert_U8_To_Str(char* res, uint8_t inputValue)
{
    if(inputValue < 10)
		{
		    res[0] = '0';
			  sprintf(&res[1], "%d", inputValue);
		}
		else sprintf(res, "%d", inputValue);
}

static void Write_Read_Pub(void)
{
		/*Test Write, Read Flash and pub data to Broker using Union*/
		W25Q16_Erase_Sector(0);

		/* Set data to write into flash */
		g_gps_data.long_t.dLongRaw = g_test_L76.dLongtitude;
		g_gps_data.lat_t.dLatRaw = g_test_L76.dLattitude;
		g_gps_data.u8Day = g_test_L76.u8Day;
		g_gps_data.u8Month = g_test_L76.u8Month;
		g_gps_data.u8Year  = g_test_L76.u8Year;
		g_gps_data.u8Hour = g_test_L76.u8Hour;
		g_gps_data.u8Minute = g_test_L76.u8Minute;
		g_gps_data.u8Second = g_test_L76.u8Second;

		Log_Info((uint8_t*)"Write Data\n", 11);
		for(uint8_t i = 0; i < 8; i++)
		{
				g_write_buffer[i] = g_gps_data.long_t.longBytes[i];
		}
		for(uint8_t i = 8; i < 16; i++)
		{
				g_write_buffer[i] = g_gps_data.lat_t.latBytes[i - 8];
		}
		g_write_buffer[16] = g_gps_data.u8Day;
		g_write_buffer[17] = g_gps_data.u8Month;
		g_write_buffer[18] = g_gps_data.u8Year;
		g_write_buffer[19] = g_gps_data.u8Hour;
		g_write_buffer[20] = g_gps_data.u8Minute;
		g_write_buffer[21] = g_gps_data.u8Second;

		W25Q16_WritePage(g_write_buffer, 0, 0x00, 22);

		HAL_Delay(500);
		Log_Info((uint8_t*)"ReadSomeByte\n", 13);
		W25Q16_ReadSomeBytes(g_read_buffer, 0x00, 22);
		/*Publish Raw data*/

		/*Test send string type to MQTT*/
		HAL_Delay(500);
		//clear data of gps variables
		g_gps_data.long_t.dLongRaw = 0;
		g_gps_data.lat_t.dLatRaw = 0;
		g_gps_data.u8Day = 0;
		g_gps_data.u8Month = 0;
		g_gps_data.u8Year = 0;
		g_gps_data.u8Hour = 0;
		g_gps_data.u8Minute = 0;
		g_gps_data.u8Second = 0;

		/*read and pub longitude*/
		memset(g_buff_send, '\0', 60); //clear buffer send

		memset(g_buff_send_long_lat, '\0', 30); //clear buffer long_lat
		for(uint8_t i = 0; i < 8; i++)
		{
				g_gps_data.long_t.longBytes[i] = g_read_buffer[i];
		}

		sprintf(g_buff_send_long_lat,  "%lf", g_gps_data.long_t.dLongRaw);
		strcat(g_buff_send, g_buff_send_long_lat);
		strcat(g_buff_send, (const char*)",");

		/*read and pub latitude*/
		memset(g_buff_send_long_lat, '\0', 30); //clear buffer long_lat
		for(uint8_t i = 0; i < 8; i++)
		{
				g_gps_data.lat_t.latBytes[i] = g_read_buffer[i+8];
		}
		sprintf(g_buff_send_long_lat,  "%lf", g_gps_data.lat_t.dLatRaw);
		strcat(g_buff_send, g_buff_send_long_lat);
		strcat(g_buff_send, (const char*)",");

		/* Read and pub date */
		memset(g_buff_send_date, '\0', 2); //clear buffer date
		g_gps_data.u8Day = g_read_buffer[16];
		Convert_U8_To_Str(g_buff_send_date, g_gps_data.u8Day);
		strcat(g_buff_send, g_buff_send_date);

		memset(g_buff_send_date, '\0', 2); //clear buffer date
		g_gps_data.u8Month = g_read_buffer[17];
		Convert_U8_To_Str(g_buff_send_date, g_gps_data.u8Month);
		strcat(g_buff_send, g_buff_send_date);

		memset(g_buff_send_date, '\0', 2); //clear buffer date
		g_gps_data.u8Year =  g_read_buffer[18];
		Convert_U8_To_Str(g_buff_send_date, g_gps_data.u8Year);
		strcat(g_buff_send, g_buff_send_date);
		strcat(g_buff_send, (const char*)",");

		/* Read and pub time */
		memset(g_buff_send_time, '\0', 2); //clear buffer time
		g_gps_data.u8Hour = g_read_buffer[19];
		Convert_U8_To_Str(g_buff_send_time, g_gps_data.u8Hour);
		strcat(g_buff_send, g_buff_send_time);

		memset(g_buff_send_time, '\0', 2); //clear buffer time
		g_gps_data.u8Minute = g_read_buffer[20];
		Convert_U8_To_Str(g_buff_send_time, g_gps_data.u8Minute);
		strcat(g_buff_send, g_buff_send_time);

		memset(g_buff_send_time, '\0', 2); //clear buffer time
		g_gps_data.u8Second = g_read_buffer[21];
		Convert_U8_To_Str(g_buff_send_time, g_gps_data.u8Second);
		strcat(g_buff_send, g_buff_send_time);

		MQTT_Publish(0, 0, 0, 1, (uint8_t*)"qn052289@gmail.com/topic1", strlen(g_buff_send), (uint8_t*)g_buff_send);
}

void Wakeup_CallBack(void)
{
	/* Reconfig the system clock after waking up from stop mode */
	SystemClock_Config();

	printf("Wake up from stop mode\r\n");

	/* Resume Tick Count */
	HAL_ResumeTick();

	/* GPS enable */
	gps_power_EnOrDi(ENABLE);

	/* LTE RX pin enable */
	softUART_ReInit();

	/* LTE enable */
	Enable_LTE();

	/* Wait until the LTE finishs initialization */
	HAL_Delay(15000);

	/* Config the MQTT */
	MQTT_Config();

	#if TEST_CONNECT == 1
		/* Check whether the mqtt server is connected or not. If not, reconnected! */
		if(MQTT_Check_Connect() != RESPONSE_OK)
		{
			MQTT_Open_Connect();
			HAL_Delay(500);
		}
	#endif

	/* Print log */
	printf("!!!!MOTION DETECTED !!!!!\n\r");

	/* Quectel initialization */
	Quectel_Init();

	/* Change 2 flags */
	g_bIsMotion = false;
	g_bIsSetGPS = true;
	g_bIsStop = false;
}

void Stop_Callback(void)
{
	printf("MOTION NOT DETECTED YET !!\r\n");

	/* Close a Network for MQTT Client */
	MQTT_Close(0);

	/* Power off Module LTE */
	LTE_PWRCRL_OFF();

	/* LTE POWER SUPPY DISABLE */
	LTE_Disable();

	/* Disable GPS Module */
	gps_power_EnOrDi(DISABLE);

	/* DeInit the RX pin of UART */
	softUART_DeInit();

	/* Enter sleep mode */
	printf("Enter Stop Mode:\r\n");

	g_bIsStop = true;

	/* Stop tick count */
	HAL_SuspendTick();

	/* Enter stop mode after 5 minutes without motion */
	HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
}

void Data_Process(void)
{
	char test_GNGGA[100] = {0};
	char test_GNRMC[100] = {0};

	/* If the motion is detected, the GPS will get data and then publish the data to MQTT server */

	/* Ensure that the power of GPS module is turned on */
	gps_power_EnOrDi(ENABLE);

	printf("\r\n****START THE CONVERSION******\r\n\n");

	/* Receive the string */
	HAL_UART_Receive(&huart2, (uint8_t*)g_rxBuffer, sizeof(g_rxBuffer), HAL_MAX_DELAY);

	/* 2 strings to split the GNGAA from the NMEA sent from the Quectel L76 LB */
	printf("%s\r\n\n", g_rxBuffer);
	printf("%d\r\n\n", strlen(g_rxBuffer));

	/* Parse the NMEA string */
	gps_read(g_testBuffer2, &g_test_L76, test_GNGGA, test_GNRMC);

	/* Write to Flash, Read from Flash and then publish the infos to MQTT serser */
	Write_Read_Pub();
}


void System_Initialization(void)
{
	/* Flash memory init */
	Flash_Init();

	/* Acce init */
	g_u8Test = SC7A20_Init();

	/* Enable UART2 Receive Interrupt */
	HAL_UART_Receive_IT(&huart2, (uint8_t*)g_rxBuffer, sizeof(g_rxBuffer));

	/* Initialize GPS module */
	Quectel_Init();

	/* Enable the power of LTE module */
	Enable_LTE();

	/* Delay to let the LTE modules finishs initializing */
	HAL_Delay(15000);
}

