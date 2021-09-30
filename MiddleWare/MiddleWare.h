#ifndef _MIDDLEWARE_H
#define _MIDDLEWARE_H

#include "stm32f4xx_hal.h"
#include "soft_uart.h"
#include "stm32f4xx_hal_uart_emul.h"
#include "lte.h"
#include "w25q16.h"

/*Macro for Bootloader*/
#define APP_ADDRESS 		(uint32_t) 0x08020000
#define APP_SECTOR			FLASH_SECTOR_5

/**
  * @brief  struct of a hex line
  */
typedef struct HexStruct	
{
	__IO uint8_t byteCount;//1byte
	__IO uint16_t addRess;	//2byte address
	__IO uint8_t recordType; //1byte 
	__IO uint32_t u32data[4];	//4*4 = 16byte
	__IO uint8_t checkSum; //1byte
} HexTypedef_t;

/**
  * @brief  Get a line in hex file, copy to buffer
	* @param  targetBuff:  buff target to pass data
	* @param  addrStart: address of start data in external flash
  * @retval address of new start data in external flash(stop data + 1)
  */
uint32_t Get_Hex_Line(uint8_t* targetBuff, uint32_t addrStart);

/**
  * @brief  Convert ascii to decimal
	* @param  character:  character input
  * @retval value in decimal
  */
uint8_t Ascii_to_Dec(uint8_t character);

/**
  * @brief  Write a buffer to internal flash
	* @param  none
  * @retval none
  */
void Write_Flash(void);

/**
  * @brief  Process a line in hex file
	* @param  line_hex_buff:  buff include data of a hex line
	* @param  addrStart: address of start data in external flash
  * @retval 0(check sum false or not flash data) or 1(check sum true)
  */
uint8_t Process_Hex_Line(uint8_t* line_hex_buff);

/**
  * @brief  Boot function, jump to new firmware 
	* @param  none
  * @retval none
  */
void Boot(void);

#endif

/*end of file*/
