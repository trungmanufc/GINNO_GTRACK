/*
 * SC7A20.C
 *
 *  Created on: Jul 15, 2021
 *      Author: Admin
 */




#include "SC7A20.h"

static void SC7A20_write(uint8_t u8Value, uint8_t u8RegAddress);
static void SC7A20_setMotionTHS(uint8_t u8Threshold);
static void SC7A20_setInt1Duration(uint8_t u8Time);


extern I2C_HandleTypeDef hi2c1;

uint8_t SC7A20_Init()
{
	/*1. Turn on the sensor with ODR = 400Hz normal mode*/
	SC7A20_write(SC7A20_CR1_NORMAL_400HZ_EN, SC7A20_ADDR_CTRL_REG1);

	/*2. Enable AOI1 Interrupt SIGNAL in INT1*/
	SC7A20_write(SC7A20_CR3_ENABLE_AOI_INT1, SC7A20_ADDR_CTRL_REG3);

	/*3. Enable Reset after Interrupt event and filter used*/
	SC7A20_write(SC7A20_CR2_INT_RST_FILTER, SC7A20_ADDR_CTRL_REG2);

	/*4. Detection */
	SC7A20_write(SC7A20_CR5_4D_DISABLE, SC7A20_ADDR_CTRL_REG5);

	/*5. Enable 16G range of the SC7A20*/
	SC7A20_write(SC7A20_CR4_2G, SC7A20_ADDR_CTRL_REG4);

	/*6. Enable Interrupt 1 for Motion detection*/
	SC7A20_write(SC7A20_INT1_MOTION_DETECT, SC7A20_ADDR_INT1_CFG);

	/*7. Set motion detection Threshold */
	SC7A20_setMotionTHS(16);

	/*8. Set duration of INT1 to 10*/
	SC7A20_setInt1Duration(1);

	/*10. Read back the data from CTRL1, if the data is successfully writen, the initialization is successful*/
	if (SC7A20_read(SC7A20_ADDR_CTRL_REG1) == SC7A20_CR1_NORMAL_400HZ_EN)
	{
		return SC7A20_read(SC7A20_ADDR_CTRL_REG1);
	}else
	{
		return HAL_ERROR;
	}

}

void SC7A20_coordinate_read(Coordinate_t* pCoordinate)
{

	/*1. Read the OUT_X_L register */
	pCoordinate->u16XCoor = ((SC7A20_read(SC7A20_ADDR_OUT_X_H) << 8) | SC7A20_read(SC7A20_ADDR_OUT_X_L));
	/*2. Read the OUT_Y_L register */
	pCoordinate->u16YCoor = ((SC7A20_read(SC7A20_ADDR_OUT_Y_H) << 8) | SC7A20_read(SC7A20_ADDR_OUT_Y_L));
	/*3. Read the OUT_Z_L register */
	pCoordinate->u16ZCoor = ((SC7A20_read(SC7A20_ADDR_OUT_Z_H) << 8) | SC7A20_read(SC7A20_ADDR_OUT_Z_L));

}

static void SC7A20_write(uint8_t u8Value, uint8_t u8RegAddress)
{
	/* Write the address of the register and then write the data */
	uint8_t u8Tx[2];
	u8Tx[0] = u8RegAddress;
	u8Tx[1] = u8Value;
	HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)SC7A20_I2C_ADDRESS, u8Tx, 2, HAL_MAX_DELAY);
}

uint8_t SC7A20_read(uint8_t u8RegAddress)
{
	uint8_t u8Data = 0;

	/*1. First send the address of the register we want to read*/
	HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)SC7A20_I2C_ADDRESS, &u8RegAddress, 1, HAL_MAX_DELAY);

	/*2. Receive the data of the register we want to read*/
	HAL_I2C_Master_Receive(&hi2c1, (uint16_t)SC7A20_I2C_ADDRESS, &u8Data, 1, HAL_MAX_DELAY);

	return u8Data;
}

static void SC7A20_setMotionTHS(uint8_t u8Threshold)
{
	SC7A20_write(u8Threshold, SC7A20_ADDR_INT1_THS);
}

static void SC7A20_setInt1Duration(uint8_t u8Time)
{
	SC7A20_write(u8Time, SC7A20_ADDR_INT1_DURATION);
}
