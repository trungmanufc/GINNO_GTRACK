#ifndef _W25Q16_H_
#define _W25Q16_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

/***************Define for Debug****************/
#define FLASH_DEBUG					0

/***************Define for UART****************/
#define UART_LOG				huart1

/***************Define for SPI****************/
#define W25Q16_SPI				hspi2
#define W25Q16_CS_PORT			GPIOB
#define W25Q16_CS_PIN			GPIO_PIN_12
#define	W25Q16_CS_EN			HAL_GPIO_WritePin(W25Q16_CS_PORT, W25Q16_CS_PIN, GPIO_PIN_RESET)
#define	W25Q16_CS_DIS			HAL_GPIO_WritePin(W25Q16_CS_PORT, W25Q16_CS_PIN, GPIO_PIN_SET)

/***************Define for instructions****************/
#define DUMMY_BYTE				0xA5
#define READ_UNIQUE_ID 			0x4B
#define WRITE_ENABLE			0x06
#define WRITE_DISABLE			0x04
#define FAST_READ				0x0B
#define PAGE_PROGRAM			0x02
#define SECTOR_ERASE			0x20
#define BLOCK_ERASE				0xD8
#define CHIP_ERASE				0xC7
#define READ_STATUS_REG1		0x05
#define READ_STATUS_REG2		0x35
#define READ_STATUS_REG3		0x15

/***************Define for parameters of W25Q16****************/
#define NUM_BLOCK					32
#define NUM_SECTOR				(NUM_BLOCK * 16)
#define NUM_PAGE					(NUM_SECTOR * 16)

#define SIZE_PAGE					256
#define SIZE_SECTOR				(SIZE_PAGE * 16)
#define SIZE_BLOCK				(SIZE_SECTOR * 16)


/*!
 * @brief Defines the w25q16 structure
 *
 * This structure is used for w25q16 flash ic
 */
typedef enum
{
		IDLE = 0U,
		BUSY,
} busy_t;

/*!
 * @brief Defines the w25q16 structure
 *
 * This structure is used for w25q16 flash ic
 */
typedef struct
{
		uint8_t   uniqID[8];		/*!<Unique ID Field */
		uint16_t	sizePage;			/*!<size of a page Field */
		uint16_t 	numPage;			/*!<number of pages Field */
		uint16_t	sizeSector;		/*!<size of a sector Field */
		uint16_t 	numSector;		/*!<number of sectors Field */
		uint32_t	sizeBlock;		/*!<size of a block Field */
		uint16_t 	numBlock;			/*!<number of blocks Field */
		uint8_t		statusReg1;		/*!<status register 1 Field */
		uint8_t		statusReg2;		/*!<status register 2 Field */
		uint8_t		statusReg3;		/*!<status register 3 Field */
		busy_t 	  isBusy;				/*!<check busy status Field */

}	w25q16_t;

/**
  * @brief  Sends an amount of data in blocking mode.
	*
  * @param  sizeStr  Amount of data elements to be sent
  * @param  inputStr pointer to data elements to be sent
  * @retval none
  */
void Log_Info(uint8_t* inputStr,  uint8_t sizeStr);

/*!
 * @brief transmit and receive data via SPI
 *
 *
 * @param[in] data: value to be sent
 */
uint8_t W25Q16_Trans_Recv(uint8_t data);

/*!
 * @brief Read Unique ID of W25Q16
 *
 * This function reads Unique ID of W25Q16
 *
 * @param[in] none
 */
void W25Q16_Read_UniqID(void);


/*!
 * @brief enable write operation with W25Q16
 *
 * @param[in] none
 */
void W25Q16_Enable_Write(void);

/*!
 * @brief disable write operation with W25Q16
 *
 * @param[in] none
 */
void W25Q16_Disable_Write(void);

/*!
 * @brief read status register of W25Q16
 *
 * @param[in] regID: 1 or 2 or 3 corresponding to id register
 */
uint8_t W25Q16_Read_StatusReg(uint8_t regID);

/*!
 * @brief wait for end busy
 *
 * @param[in] none
 */
void W25Q16_Wait_EndBusy(void);

/*!
 * @brief Initializes the W25Q16 IC
 *
 * This function initializes the IC
 *
 * @param[in] none
 */
bool W25Q16_Init(void);

/*!
 * @brief erase full chip
 *
 * @param[in] none
 */
void W25Q16_Erase_Chip(void);

/*!
 * @brief erase sector
 *
 * @param[in] idSector: id of sector to erase (0,1,...511)
 */
void W25Q16_Erase_Sector(uint16_t idSector);

/*!
 * @brief erase block
 *
 * @param[in] idBlock: id of sector to erase (0,1,...31)
 */
void W25Q16_Erase_Block(uint8_t idBlock);

/*!
 * @brief convert id of page to id of sector
 *
 * @param[in] idPage: id of page to convert (0,1,...8191)
 */
uint16_t W25Q16_Page2Sector(uint8_t idPage);

/*!
 * @brief convert id of page to id of block
 *
 * @param[in] idPage: id of page to convert (0,1,...8191)
 */
uint16_t W25Q16_Page2Block(uint8_t idPage);

/*!
 * @brief convert id of sector to id of block
 *
 * @param[in] idSector: id of page to convert (0,1,...511)
 */
uint16_t W25Q16_Sector2Block(uint8_t idSector);

/*!
 * @brief convert id of sector to id of page
 *
 * @param[in] idSector: id of page to convert (0,1,...511)
 */
uint16_t W25Q16_Sector2Page(uint8_t idSector);

/*!
 * @brief convert id of block to id of page
 *
 * @param[in] idBlock: id of page to convert (0,1,...31)
 */
uint16_t W25Q16_Block2Page(uint8_t idBlock);

/*!
 * @brief write a byte into flash
 *
 * @param[in] data: data to write into flash
 * @param[in] addrToWrite: address of byte to write
 */
void W25Q16_WriteByte(uint8_t data,
											uint32_t addrToWrite);

/*!
 * @brief write data into a page
 *
 * @param[in] pData: pointer to data buffer
 * @param[in] idPage: id of pages (0,1,...8191)
 * @param[in] number: number of datas to write
 * @param[in] offSet: offset value of start address (byte)
 */
void W25Q16_WritePage(uint8_t* pData,
											uint16_t idPage, uint8_t offSet, uint8_t number);

/*!
 * @brief write data into a sector
 *
 * @param[in] pData: pointer to data buffer
 * @param[in] idSector: id of pages (0,1,...511)
 * @param[in] number: number of datas to write (bytes)
 * @param[in] offSet: offset value of address Sector (byte)
 */
//void W25Q16_WriteSector(uint8_t* pData,
//												uint16_t idSector, uint8_t offSet, uint8_t number);

/*!
 * @brief write data into a block
 *
 * @param[in] pData: pointer to data buffer
 * @param[in] idBlock: id of block (0,1,...31)
 * @param[in] number: number of datas to write (bytes)
 * @param[in] offSet: offset value of address Block (byte)
 */
//void W25Q16_WriteBlock(uint8_t* pData,
//											 uint16_t idBlock, uint8_t offSet, uint8_t number);

/*!
 * @brief read a byte from flash
 *
 * @param[in] pBuffer: pointer to buffer to contain from flash
 * @param[in] addrToRead: address of byte to read
 */
void W25Q16_ReadByte(uint8_t* pBuffer,
										 uint32_t addrToRead);

/*!
 * @brief read some bytes from flash
 *
 * @param[in] pBuffer: pointer to buffer to contain from flash
 * @param[in] addrToRead: address of byte to write
 * @param[in] number: number of bytes to read
 */
void W25Q16_ReadSomeBytes(uint8_t* pBuffer,
													uint32_t addrToRead, uint32_t number);

/*!
 * @brief read some bytes of page from flash
 *
 * @param[in] pBuffer: pointer to buffer to contain from flash
 * @param[in] idPage: id of page to read data (0,1,...8191)
 * @param[in] number: number of bytes to read
 * @param[in] offSet: offset value of address Page (byte)
 */
void W25Q16_ReadPage(uint8_t* pBuffer,
										 uint32_t idPage, uint8_t offSet, uint32_t number);

/*!
 * @brief read some bytes of sector from flash
 *
 * @param[in] pBuffer: pointer to buffer to contain from flash
 * @param[in] idSector: id of page to read data (0,1,...511)
 * @param[in] number: number of bytes to read
 * @param[in] offSet: offset value of address Page (byte)
 */
//void W25Q16_ReadSector(uint8_t* pBuffer,
//											 uint32_t idSector, uint8_t offSet, uint32_t number);

/*!
 * @brief read some bytes of sector from flash
 *
 * @param[in] pBuffer: pointer to buffer to contain from flash
 * @param[in] idBlock: id of page to read data (0,1,...31)
 * @param[in] number: number of bytes to read
 * @param[in] offSet: offset value of address Block (byte)
 */
//void W25Q16_ReadBlock(uint8_t* pBuffer,
//										  uint32_t idBlock, uint8_t offSet, uint32_t number);

/*Functions relate convert float to string*/

/*!
 * @brief reverse a string
 *
 * @param[in] str: pointer to input string
 * @param[in] len: length of string
 * @retval: none
 */
void reverse(char* str, int len);

/*!
 * @brief convert a int number to string
 *
 * @param[in] inputNum: input number type int
 * @param[in] str: buffer input to get string
 * @param[in] numDigit: number of digits want to output
 * @retval: number of digits output
 */
uint8_t intToStr(uint32_t inputNum, char* str, uint8_t numDigit);

/*!
 * @brief Converts a floating-point/double number to a string
 *
 * @param[in] number: input number type double/float
 * @param[in] res: restrict to get string
 * @param[in] afterPoint: number of digits want to after point
 * @retval none
 */
void ftoa(double number, char* res, uint8_t afterPoint);

#endif

/*****************EOF*****************/
