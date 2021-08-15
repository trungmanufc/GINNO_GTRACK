/*
 * L76.c
 *
 *  Created on: Jul 20, 2021
 *      Author: Tuan_Trung
 */

#include<string.h>
#include"L76.h"

extern UART_HandleTypeDef uartGPS;

/* Static function prototypes */
static void L76_Lat_Parse(char* sLat, L76* pL76, uint8_t u8NorS);
static void L76_Long_Parse(char* sLat, L76* pL76, uint8_t u8EorW);
static void L76_Time_Parse(char* sUtcTime, L76* pL76);
static void L76_Date_Parse(char* sRmcDate, L76* pL76Handle);

uint8_t Quectel_Init(void)
{

	char cGpsOnly[100] = "$PMTK353,1,0,0,0,0*2B\r\n";

	char cGps10HzFix[100] = "$PMTK220,100*1F\r\n";

	HAL_UART_Transmit_IT(&uartGPS, (uint8_t*)cGps10HzFix, strlen(cGps10HzFix));

	if(HAL_UART_Transmit_IT(&uartGPS, (uint8_t*)cGpsOnly, strlen(cGpsOnly)) != HAL_OK)
	{
		return HAL_ERROR;
	}
	else
	{
		return HAL_OK;
	}
}

void gps_read(char*	sRxBuffer, L76* pL76, char *seGNGGA, char* seGNRMC)
{

	uint8_t u8IsFix = 0;
	uint8_t u8IndexOfComma[20] = {0};
	uint8_t u8IndexOfComma2[20] = {0};
	char sUtcTime[15] = {0};
	char sDate[10]	  = {0};

	memset(pL76, 0, sizeof(L76));

	/* 2 strings to split the GNGAA and GNRMC from the NMEA sent from the Quectel L76 LB */
	char *sSubGPS  = strstr(sRxBuffer, "$GNGGA");
	char* sSubGPS2 = strstr(sRxBuffer, "$GNRMC");

	/* Split the GNRMC from the string above */
	for (int i = 0; i < strlen(sSubGPS2); i++)
	{
		if (sSubGPS2[i] == '\r')
		{
			for(int j = 0; j < i; j++)
			{
				seGNRMC[j] = sSubGPS2[j];
			}
			seGNRMC[i] = '\0';
			break;
		}
	}

	/* Test */
	printf("the string of the subGNRMC: %s\r\n", seGNRMC);

	/* Split the GNGGA from the string above */
	for (int i = 0; i < strlen(sSubGPS); i++)
	{
		if (sSubGPS[i] == '\r')
		{
			for(int j = 0; j < i; j++)
			{
				seGNGGA[j] = sSubGPS[j];
			}
			seGNGGA[i] = '\0';
			break;
		}
	}

	/* Test */
	printf("the string of the subGNGGA: %s\r\n", seGNGGA);

	int j = 0;

	/* Find the index of each comma in GNGGA string */
	for (int i = 0; i < strlen(seGNGGA); i++)
	{
		if (seGNGGA[i] == ',')
		{
			u8IndexOfComma[j++] = i;
		}
	}

	j = 0;

	/* Find the index of each comma in GNRMC string */
	for (int i = 0; i < strlen(seGNRMC); i++)
	{
		if (seGNRMC[i] == ',')
		{
			u8IndexOfComma2[j++] = i;
		}
	}

	/* Check whether the GPS is fix or not on GNGGA */
	if (seGNGGA[u8IndexOfComma[5] + 1] == '0' || strlen(seGNGGA) == 0)
	{
		if (seGNGGA[u8IndexOfComma[5] + 1] == '0')
		{
			printf("GPS NOT FIX!!!\r\n");
		}
		u8IsFix = GPS_NOT_FIX;
	}else
	{
		u8IsFix = GPS_FIX;
	}

	/* If GPS is fix, Get the DATA */
	if (u8IsFix == GPS_NOT_FIX)
	{
		/* Information of NMEA */
		memset(pL76, 0, sizeof(*pL76));
		printf("GPS NOT AVAILABLE\r\n");
	}
	else
	{
		char sLat[20] = {0};
		char sN_S[3] = {0};
		/* 1. Get the String of UTCtime from the NMEA messages */
		j = 0;

		for (uint8_t i = u8IndexOfComma[0]; i < u8IndexOfComma[1]; i++)
		{
			sUtcTime[j] = seGNGGA[i+1];
			j++;
		}
		sUtcTime[j-1] = '\0';

		L76_Time_Parse(sUtcTime, pL76);

		/* 2. Get the String of Date from the NMEA GNRMC message */
		j = 0;

		for (uint8_t i = u8IndexOfComma2[8]; i < u8IndexOfComma2[9]; i++)
		{
			sDate[j] = seGNRMC[i+1];
			j++;
		}
		sDate[j-1] = '\0';

		/* Test */
		printf("DATE string: %s\r\n",sDate);

		L76_Date_Parse(sDate, pL76);

		/* 3. Get the String of Latitude from the NMEA messages */
		j = 0;

		for (uint8_t i = u8IndexOfComma[1]; i < u8IndexOfComma[2]; i++)
		{
			sLat[j] = seGNGGA[i+1];
			j++;
		}
		sLat[j-1] = '\0';

		printf("%s\n\r", sLat);

		/* Check whether the Latitude is North side or South side */
		j = 0;

		for (uint8_t i = u8IndexOfComma[2]; i < u8IndexOfComma[3]; i++)
		{
			sN_S[j] = seGNGGA[i+1];
			j++;
		}
		sN_S[j-1] = '\0';

		/* If latitude is in the North direction, the Latitude is postive. Otherwise, It is negative */
		uint8_t u8N_S = 0;

		if (sN_S[0] == 'N')
		{
			/* The latitude is positive */
			u8N_S = GPS_NORTH;
		}else if(sN_S[0] == 'S')
		{
			/* The longtitude is negative */
			u8N_S = GPS_SOUTH;
		}
		L76_Lat_Parse(sLat, pL76, u8N_S);

		/* 4. Get the String of Longtitude from the NMEA messages */
		j = 0;
		char sLong[20] = {0};

		for (uint8_t i = u8IndexOfComma[3]; i < u8IndexOfComma[4]; i++)
		{
			sLong[j] = seGNGGA[i+1];
			j++;
		}
		sLong[j-1] = '\0';

		/* Test */
		printf("%s\r\n",sLong);

		/* Check whether the Longtitude is West or East size */
		j = 0;
		char sE_W[20] = {0};

		for (uint8_t i = u8IndexOfComma[4]; i < u8IndexOfComma[5]; i++)
		{
			sE_W[j] = seGNGGA[i+1];
			j++;
		}
		sE_W[j-1] = '\0';

		/* If Longtitude is in the East direction, the Longtitude is positive. Otherwise, It is negative . */
		uint8_t u8E_W = 0;

		if (sE_W[0] == 'E')
		{
			u8E_W = GPS_EAST;
		}
		else if (sE_W[0] == 'W')
		{
			u8E_W = GPS_WEST;
		}
		L76_Long_Parse(sLong, pL76, u8E_W);

	}
}

static void L76_Lat_Parse(char* sLat, L76* pL76, uint8_t u8NorS)
{
	char degree[3];
	char minute[10];

	/* Check the postition of the dot to divide the degrees and minutes
	 * The Latitude NMEA form ddmm.mmmm
	 */

	/* 1. Parse the Degree */
	for (uint8_t i = 0; i < 2; i++)
	{
		degree[i] = sLat[i];
	}
	degree[2] = '\0';

	/* Test */
	printf("Lat Degree: %s\r\n", degree);

	double dTempDeg = atof(degree);

	/* 2. Parse the minute */
	uint8_t j = 0;

	printf("%s\r\n", sLat);

	for (uint8_t i = 2; i < (strlen(sLat) - 1); i++)
	{
		minute[j] = sLat[i];
		j++;
	}
	minute[j] = '\0';

	printf("Lat Minute: %s\r\n", minute);

	double dTempMin = atof(minute);

	/* 3. Get the latitude in form dd.xxxxx */
	if (u8NorS == GPS_NORTH)
	{
		pL76->dLattitude = dTempDeg + (dTempMin / 60);
	}
	else if (u8NorS == GPS_SOUTH)
	{
		pL76->dLattitude = -(dTempDeg + (dTempMin / 60));
	}
	else
	{
		//Unavailable
		;
	}
	printf("NMEA latitude: %lf\r\n", pL76->dLattitude);

}

static void L76_Long_Parse(char* sLong, L76* pL76, uint8_t u8EorW)
{
	char degree[4];
	char minute[10];

	/* Check the position of the dot to divide degrees and minutes
	 * The Longtitude NMEA form dddmm.mmmm
	 */

	/* 1. Parse the degrees */
	for (uint8_t i = 0; i < 3; i++)
	{
		degree[i] = sLong[i];
	}
	degree[3] = '\0';

	/* Test */
	printf("Long Degree: %s\r\n", degree);

	double dTempDeg = atof(degree);

	/* 2. Parse the minutes */
	uint8_t j = 0;

	for (uint8_t i = 3; i < (strlen(sLong) - 1); i++)
	{
		minute[j] = sLong[i];
		j++;
	}
	minute[j] = '\0';

	/* Test */
	printf("Long minute: %s\r\n", minute);

	double dTempMin = atof(minute);

	/* 3. Get the latitude in form of ddd.xxxxx */
	if (u8EorW == GPS_EAST)
	{
		pL76->dLongtitude = dTempDeg + (dTempMin / 60);
	}
	else if (u8EorW == GPS_WEST)
	{
		pL76->dLongtitude = -(dTempDeg + (dTempMin / 60));
	}
	else
	{
		//Unavailable
		;
	}

	/* Test */
	printf("NMEA longtitude: %lf\r\n", pL76->dLongtitude);

}

static void L76_Time_Parse(char* sUtcTime, L76* pL76)
{
	uint32_t dTime = (uint32_t)atof(sUtcTime);

	/* 1. Get the seconds */
	pL76->u8Second = (uint8_t) (dTime % 100);

	/* 2. Get the minutes */
	pL76->u8Minute = (uint8_t) ((dTime / 100) % 100);

	/* 3. Get the hour */
	pL76->u8Hour = (uint8_t) (dTime / 10000);

	/* Test */
	printf("UTC TIME: %d:%d:%d\r\n", pL76->u8Hour, pL76->u8Minute, pL76->u8Second);
}

static void L76_Date_Parse(char* sRmcDate, L76* pL76Handle)
{
	uint32_t dDate = (uint32_t)atof(sRmcDate);

	/* 1. Get the day */
	pL76Handle->u8Day = (uint8_t) (dDate / 10000);

	/* 2. Get the month */
	pL76Handle->u8Month = (uint8_t) ((dDate / 100) % 100);

	/* 3. Get the year */
	pL76Handle->u16Year = (uint16_t) (2000 + (dDate % 100));

	/* Test */
	printf("Date: %d/%d/%d \r\n", pL76Handle->u8Day, pL76Handle->u8Month, pL76Handle->u16Year);

}
