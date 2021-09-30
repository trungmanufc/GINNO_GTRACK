/*
 * gv.h
 *
 * THIS FILE CONTAINS GLOBAL VARIBLES OF THE SYSTEM
 *
 *
 *  Created on: Sep 29, 2021
 *      Author: Admin
 */

#ifndef GV_H_
#define GV_H_



uint8_t g_u8SC7A20Error;
uint8_t g_u8I2cTest;
uint8_t g_u8Test;
uint8_t g_u8ReceiveTest;
uint8_t g_u8Data;
Coordinate_t g_testXYZ;
uint8_t g_isMotion;
uint8_t g_u8StatusInt1;
bool g_bIsMotion;
bool g_bIsInMotion;
bool g_bIsSetGPS;
bool g_bIsStop;
uint32_t g_u32CurrentTime;


/* Quang's global variables */
uint8_t 		g_buffer_log[30] ;
uint8_t 		g_write_buffer[MAX_SIZE_BUFF];
uint8_t			g_read_buffer[MAX_SIZE_BUFF];
uint8_t 		g_data_write;
uint8_t 		g_data_read;
GPS_t   		g_gps_data;
char 			g_rxbuffer2[1000];

char 			g_buff_send[60];
char 			g_buff_send_long_lat[20];
char			g_buff_send_year[2] ;
char 			g_buff_send_date[2];
char 			g_buff_send_time[2];
uint8_t 		g_day, g_month, g_hour, g_minute, g_second, g_year;
double 			g_long;
double 			g_lat;


response_t 		g_flag ;

/* Global variables for LTE module */
uint8_t    		g_recv_byte, g_count, g_count_temp ;
uint8_t 		g_isDone , g_check_end;
uint8_t 		g_recv_buff[MAX_SIZE_BUFF];
uint8_t 		g_buff_temp[MAX_SIZE_BUFF];
uint8_t 		g_id_msg, g_size_sms, g_size_IP;
uint8_t 		g_mqtt_isOn;
uint32_t 		g_timeNow;


/* Sample NMEA string to test GPS parse function */
char* g_testBuffer2 ;

/* The Rx Buffer from the Quectel L76 LB */
char g_rxBuffer[1000];
char g_rxBuffer2[1000];

/* Sample NMEA string to test GPS parse function */
char *g_testBuffer ;

L76 g_test_L76;

#endif /* GV_H_ */
