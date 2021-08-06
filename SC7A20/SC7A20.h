/*
 * SC7A20.h
 *
 *  Created on: Jul 15, 2021
 *      Author: Admin
 */

#ifndef SC7A20_H_
#define SC7A20_H_

#include"stm32f4xx_hal.h"

/*Application configurable items */
#define SC7A20_I2C					I2C1
#define SC7A20_I2C_GPIO_PORT 		GPIOB
#define SC7A20_SDA_PIN				GPIO_PIN_7
#define SC7A20_SCL_PIN				GPIO_PIN_6
//#define SC7A20_I2C_SPEED 			I2C_SPEED_STANDARD()


/*Slave Address of SC7A20*/
#define SC7A20_I2C_ADDRESS			0x30

/*Register Address of SC7A20*/
#define SC7A20_ADDR_OUT_TEMP_L		0x0C
#define SC7A20_ADDR_OUT_TEMP_H		0x0D
#define SC7A20_ADDR_WHO_AM_I		0x0F
//#define SC7A20_ADDR_USER_CAL
#define SC7A20_ADDR_NVM_WR			0x1E
#define SC7A20_ADDR_TEMP_CFG		0x1F
#define SC7A20_ADDR_CTRL_REG1 		0x20
#define SC7A20_ADDR_CTRL_REG2		0x21
#define SC7A20_ADDR_CTRL_REG3		0x22
#define SC7A20_ADDR_CTRL_REG4		0x23
#define SC7A20_ADDR_CTRL_REG5		0x24
#define SC7A20_ADDR_CTRL_REG6		0x25
#define SC7A20_ADDR_REFERENCE		0x26
#define SC7A20_ADDR_STATUS			0x27
#define SC7A20_ADDR_OUT_X_L			0x28
#define SC7A20_ADDR_OUT_X_H			0x29
#define SC7A20_ADDR_OUT_Y_L			0x2A
#define SC7A20_ADDR_OUT_Y_H			0x2B
#define SC7A20_ADDR_OUT_Z_L			0x2C
#define SC7A20_ADDR_OUT_Z_H			0x2D
#define SC7A20_ADDR_FIFO_CTRL		0x2E
#define SC7A20_ADDR_FIFO_SRC		0x2F
#define SC7A20_ADDR_INT1_CFG		0x30
#define SC7A20_ADDR_INT1_SOURCE		0x31
#define SC7A20_ADDR_INT1_THS		0x32
#define SC7A20_ADDR_INT1_DURATION	0X33
#define SC7A20_ADDR_INT2_CFG		0x34
#define SC7A20_ADDR_INT2_SOURCE		0x35
#define SC7A20_ADDR_INT2_THS		0x36
#define SC7A20_ADDR_INT2_DURATION	0X37
#define SC7A20_ADDR_CLICK_CFG		0X38
#define SC7A20_ADDR_CLICK_SRC		0X39
#define SC7A20_ADDR_CLICK_THS		0X3A
#define SC7A20_ADDR_TIME_LIMIT		0X3B
#define SC7A20_ADDR_TIME_LATENCY	0X3C
#define SC7A20_ADDR_TIME_WINDOW		0X3D
#define SC7A20_ADDR_ACT_THIS		0X3E
#define SC7A20_ADDR_ACT_DURATION	0X3F


/*Some generic macros*/
#define SC7A20_CR1_NORMAL_400HZ_EN	0x73
#define SC7A20_CR3_ENABLE_AOI_INT1	0x40
#define SC7A20_INT1_MOTION_DETECT	0x0A
#define SC7A20_CR2_INT_RST_FILTER	0x00
#define SC7A20_CR5_4D_DISABLE		0x00
#define SC7A20_CR4_2G				0x38

typedef struct
{
	uint16_t u16XCoor;
	uint16_t u16YCoor;
	uint16_t u16ZCoor;
}Coordinate_t;

uint8_t SC7A20_Init(void);
void SC7A20_coordinate_read(Coordinate_t* pCoordinate);
uint8_t SC7A20_read(uint8_t u8RegAddress);




#endif /* SC7A20_H_ */
