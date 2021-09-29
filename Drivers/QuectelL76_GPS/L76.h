/*
 * L76.h
 *
 *  Created on: Jul 20, 2021
 *      Author: Admin
 */

#ifndef L76_H_
#define L76_H_

#include"stm32f4xx_hal.h"
#include<stdio.h>
#include<stdlib.h>


/** @defgroup GPS macros define
  * @brief GPS generic macros
  * @{
  */
#define GPS_NOT_FIX 0
#define GPS_FIX 	1
#define GPS_NORTH	1
#define GPS_SOUTH	0
#define GPS_EAST	1
#define GPS_WEST	0


/**
  * @brief GPS GNSS module L76 infomation structure definition
  */
typedef struct
{
	double dLongtitude;
	double dLattitude;
	uint8_t u8Day;
	uint8_t u8Month;
	uint8_t u8Year;
	uint8_t u8Hour;
	uint8_t u8Minute;
	uint8_t u8Second;
}L76;


/**
  * @brief  Initialie the GPS module.
  *
  *
  * @param  none
  *
  *
  *
  *
  * @retval Successful or failure initialization
  */
uint8_t Quectel_Init(void);


/**
  * @brief  Parse the NMEA string and save the information needed into L76 structure
  *
  * @param  sL76: pointer to GPS info structure
  * @param  sRxBuffer: pointer to the string buffer that contains NMEA string
  *
  *
  *
  * @retval none
  */
void gps_read(char*	sRxBuffer,
			  L76* sL76,
			  char *seGNGGA,
			  char* seGNRMC);


/**
  * @brief  Enable or disable the power of GPS Module
  *
  * @param  u8EnOrDi: DISABLE or ENABLE
  *
  *
  *
  * @retval none
  */
void gps_power_EnOrDi(uint8_t u8EnOrDi);



#endif /* L76_H_ */

