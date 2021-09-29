#ifndef __MIDDLEWARE_H
#define __MIDDLEWARE_H

#include "lte.h"
#include "L76.h"
#include "w25q16.h"
#include "SC7A20.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "gv.h"

/* Some macros */
#define TEST_MQTT		0
#define TEST_MQTT_SSL	1
#define TEST_FLASH 		0
#define MAX_SIZE_BUFF	256
#define TEST_GPS		1
#define TEST_CONNECT	1
#define TEST_INIT		0

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_Emul_HandleTypeDef UartEmulHandle;

/* !
 * @brief Open MQTT server
 *
 * This function send AT commands to OPEN the MQTT server and
 * check whether it is opened or not.
 *
 * @param[in] none
 *
 *
 *
 * @retval Respon success or error
 */
response_t MQTT_Open_Connect(void);




/*!
 * @brief Wakeup the system
 *
 * This function is used to reconfig the system
 * when it wakes up from interrupt
 *
 * @param[in] none
 *
 *
 *
 * @retval none
 */
void Wakeup_CallBack(void);




/*
 * @brief Puts the system into Stop Mode
 *
 *
 * This function is used to disable some peripherals
 * and put the system into Stop Mode after 5 minutes without motion
 *
 * @param[in] none
 *
 *
 * @retval none
 */
void Stop_Callback(void);


/*
 * @brief Process the data after the motion is detected
 *
 *
 * This function is used to parse the NMEA, puts the
 * parsed data to MQTT server
 *
 *
 * @param[in] none
 *
 *
 * @retval none
 */
void Data_Process(void);



/*
 * @brief Initialize some peripherals which is connected externally to the system
 *
 *
 * Initialize the Flash memory, the LTE module
 *
 *
 * @param[in] none
 *
 *
 * @retval none
 */
void System_Initialization(void);

#endif
