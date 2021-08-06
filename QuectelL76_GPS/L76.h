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

typedef struct
{
	double dLongtitude;
	double dLattitude;
	uint8_t u8Day;
	uint8_t u8Month;
	uint16_t u16Year;
	uint8_t u8Hour;
	uint8_t u8Minute;
	uint8_t u8Second;
}L76;

uint8_t Quectel_Init(void);

void gps_read(char*	sRxBuffer, L76* sL76, char *seGNGGA, char* seGNRMC);

#endif /* L76_H_ */

