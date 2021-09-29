/*
 * gv.c
 *
 *  Created on: Sep 29, 2021
 *      Author: Admin
 */
#include "Middleware.h"
#include "gv.h"

/* USER CODE BEGIN PFP */
uint8_t g_u8SC7A20Error = RESET;
uint8_t g_u8I2cTest = RESET;
uint8_t g_u8Test = SC7A20_ADDR_CTRL_REG1;
uint8_t g_u8ReceiveTest = RESET;
uint8_t g_u8Data = 0;
Coordinate_t g_testXYZ;
uint8_t g_isMotion = RESET;
//uint8_t isInMotion = RESET;
uint8_t g_u8StatusInt1 = 0;
bool g_bIsMotion = false;
bool g_bIsInMotion = false;
bool g_bIsSetGPS = false;
bool g_bIsStop = false;
uint32_t g_u32CurrentTime = 0;


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
char g_rxbuffer2[1000];

char 			g_buff_send[60] = {0};
char 			g_buff_send_long_lat[20] = {0};
char			g_buff_send_year[2] = {0};
char 			g_buff_send_date[2] = {0};
char 			g_buff_send_time[2] = {0};
uint8_t 		g_day = 18, g_month = 8, g_hour = 7, g_minute = 57, g_second = 9, g_year = 21;
double 			g_long = -1234.456789;
double 			g_lat  = -3214.876543;


response_t 		g_flag = RESPONSE_ERR;

/* Global variables for LTE module */
uint8_t    		g_recv_byte = 0, g_count = 0, g_count_temp = 0;
uint8_t 		g_isDone = RX_FALSE, g_check_end = 1;
uint8_t 		g_recv_buff[MAX_SIZE_BUFF] = {0};
uint8_t 		g_buff_temp[MAX_SIZE_BUFF] = {0};
uint8_t 		g_id_msg = 0, g_size_sms = 0, g_size_IP = 0;
uint8_t 		g_mqtt_isOn = OFF;
uint32_t 		g_timeNow = 0;

/* Sample NMEA string to test GPS parse function */
char* g_testBuffer2 = "$GNRMC,152657.000,A,2057.82025,N,10549.33270,E,0.00,0.00,060821,,,A,V*04\r\n$GNGGA,152658.000,2057.82021,N,10549.33274,E,1,12,1.0,46.7,M,-27.8,M,,*59\r\n";

/* The Rx Buffer from the Quectel L76 LB */
char g_rxBuffer[1000];

/* Sample NMEA string to test GPS parse function */
char *g_testBuffer = "$GNRMC,573843,A,3349.896,N,11808.521,W,000.0,360.0,230108,013.4,E*69\r\n$GNGGA,185833.80,4808.7402397,S,01133.9325039,W,1,15,1.1,470.50,M,45.65,M,,*75\r\n";

L76 g_test_L76;


