/*!
  ******************************************************************************
  * @file           : w25q16.c
  * @brief          : functions to work with w25q16 flash ic
  ******************************************************************************
  *
  ******************************************************************************
  */

#include "w25q16.h"
#include "stm32f4xx_hal_uart_emul.h"

/****************Global variables****************/
extern SPI_HandleTypeDef  W25Q16_SPI;
extern UART_HandleTypeDef UART_LOG;
w25q16_t myDevice;
/****************FUNCTIONS*****************/

/**
  * @brief  Sends an amount of data in blocking mode.
	*
  * @param  sizeStr  Amount of data elements to be sent
  * @param  inputStr pointer to data elements to be sent
  * @retval none
  */
//void Log_Info(uint8_t* inputStr,  uint8_t sizeStr)
//{
//		if(inputStr != NULL)
//		{	
//		    HAL_UART_Transmit(&UART_LOG, inputStr, sizeStr, 1000);
//		}
//		else 
//		{
//				HAL_UART_Transmit(&UART_LOG, (uint8_t*)"Data Log NULL\n", 14, 1000);
//		}
//}

/*!
 * @brief transmit and receive data via SPI 
 *
 *
 * @param[in] data: value to be sent
 */
uint8_t W25Q16_Trans_Recv(uint8_t data)
{
		uint8_t retValue = 0;	//local variable to return
		HAL_SPI_TransmitReceive(&W25Q16_SPI, &data, &retValue, 1, 100);
		return retValue;
}

/*!
 * @brief Read Unique ID of W25Q16 
 *
 * This function reads Unique ID of W25Q16 
 *
 * @param[in] none
 */
void W25Q16_Read_UniqID(void)
{
		/*!
		****************Step to follow**************************************************************
		1. Enable SPI
		2. Send READ_UNIQUE_ID Instruction
		3. Send 4 dummy bytes
		4. Send 8 dummy byes and receive 8byte uniqueID
		5. Disable SPI
		******************************************************************************
		*/
		W25Q16_CS_EN; //Enable SPI
		W25Q16_Trans_Recv(READ_UNIQUE_ID); //Send READ_UNIQUE_ID Instruction
		for(uint8_t i = 0; i < 4; i++) //Send 4 dummy bytes
		{
				W25Q16_Trans_Recv(DUMMY_BYTE);
		}
		for(uint8_t i = 0; i < 8; i++) //Send 8 dummy byes and receive 8byte uniqueID
		{
			myDevice.uniqID[i] = W25Q16_Trans_Recv(DUMMY_BYTE);
		}
		W25Q16_CS_DIS; //Disable SPI
}

/*!
 * @brief enable write operation with W25Q16 
 *
 * @param[in] none
 */
void W25Q16_Enable_Write(void)
{
		/*!
		****************Step to follow**************************************************************
		1. Enable SPI
		2. Send WRITE_ENABLE Instruction
		3. Disable SPI
		******************************************************************************
		*/
		W25Q16_CS_EN;
		W25Q16_Trans_Recv(WRITE_ENABLE);
		W25Q16_CS_DIS;
		HAL_Delay(1);
}

/*!
 * @brief disable write operation with W25Q16 
 *
 * @param[in] none
 */
void W25Q16_Disable_Write(void)
{
		/*!
		****************Step to follow**************************************************************
		1. Enable SPI
		2. Send WRITE_DISABLE Instruction
		3. Disable SPI
		******************************************************************************
		*/
		W25Q16_CS_EN;
		W25Q16_Trans_Recv(WRITE_DISABLE);
		W25Q16_CS_DIS;
		HAL_Delay(1);	
}

/*!
 * @brief read status register of W25Q16
 *
 * @param[in] regID: 1 or 2 or 3 corresponding to id register
 */
uint8_t W25Q16_Read_StatusReg(uint8_t regID)
{
		/*!
		****************Step to follow**************************************************************
		1. Enable SPI
		2. Check ID of Register
		3. Send READ_STATUS_REG corresponding
		4. Disable SPI
		******************************************************************************
		*/
		uint8_t retValue = 0;	//local variable to return
		W25Q16_CS_EN;
		switch(regID)
		{
				case 1:
				{
						W25Q16_Trans_Recv(READ_STATUS_REG1);
						retValue = W25Q16_Trans_Recv(DUMMY_BYTE);
						myDevice.statusReg1 = retValue;
						break;
				}
				case 2:
				{
						W25Q16_Trans_Recv(READ_STATUS_REG2);
						retValue = W25Q16_Trans_Recv(DUMMY_BYTE);
						myDevice.statusReg2 = retValue;
						break;
				}
				default:
						W25Q16_Trans_Recv(READ_STATUS_REG3);
						retValue = W25Q16_Trans_Recv(DUMMY_BYTE);
						myDevice.statusReg3 = retValue;
						break;
		} //end switch
		W25Q16_CS_DIS; //disable SPI
		
		return retValue;
}

/*!
 * @brief wait for end busy
 *
 * @param[in] none
 */
void W25Q16_Wait_EndBusy(void)
{
		/*!
		****************Step to follow**************************************************************
		1. Enable SPI
		2. Send READ_STATUS_REG1 
		3. Wait until MSB bit of status register 1 != 1
		4. Disable SPI
		******************************************************************************
		*/
		HAL_Delay(1);
		W25Q16_CS_EN;	//Enable SPI
		W25Q16_Trans_Recv(READ_STATUS_REG1); //Send READ_STATUS_REG1
		do //Wait until MSB bit of status register 1 != 1
		{
				myDevice.statusReg1 = W25Q16_Trans_Recv(DUMMY_BYTE);
				HAL_Delay(1);
		}while((myDevice.statusReg1 & 0x01) == 0x01);
		W25Q16_CS_DIS; //Disable SPI
}

/*!
 * @brief Initializes the W25Q16 IC
 *
 * This function initializes the IC
 *
 * @param[in] none
 */
bool W25Q16_Init(void)
{
		/*!
		****************Step to follow**************************************************************
		1. Set busy status for device
		2. Disable SPI
		3. Config parameters of Device
		4. Wait until MSB bit of status register 1 != 1
		5. Disable SPI
		6. Set idle status for device
		******************************************************************************
		*/
		char buffer_to_log[20];	//buffer for data to log via uart
	
		myDevice.isBusy = BUSY; //Set busy status for device
		W25Q16_CS_DIS; //Disable SPI
		
		/*Config parameters of Device*/
		myDevice.numBlock   = NUM_BLOCK;
		myDevice.numSector  = NUM_SECTOR;
		myDevice.numPage		= NUM_PAGE;
		myDevice.sizePage   = SIZE_PAGE;
		myDevice.sizeSector = SIZE_SECTOR;
		myDevice.sizeBlock  = SIZE_BLOCK;
	
		W25Q16_Read_UniqID();
		W25Q16_Read_StatusReg(1);
		W25Q16_Read_StatusReg(2);
		W25Q16_Read_StatusReg(3);
		
		
		/*Log informations of device*/
		Log_Info((uint8_t*)"Initialize\n", 12);
		sprintf((char*)buffer_to_log, "NumBlock = %d\n", myDevice.numBlock);
		Log_Info((uint8_t*)buffer_to_log, 15);
		sprintf((char*)buffer_to_log, "NumSector = %d\n", myDevice.numSector);
		Log_Info((uint8_t*)buffer_to_log, 16);
		sprintf((char*)buffer_to_log, "NumPage = %d\n", myDevice.numPage);
		Log_Info((uint8_t*)buffer_to_log, 15);
		sprintf((char*)buffer_to_log, "SizePage = %d\n", myDevice.sizePage);
		Log_Info((uint8_t*)buffer_to_log, 15);
		sprintf((char*)buffer_to_log, "SizeSector = %d\n", myDevice.sizeSector);
		Log_Info((uint8_t*)buffer_to_log, 18);
		sprintf((char*)buffer_to_log, "SizeBlock = %d\n", myDevice.sizeBlock);
		Log_Info((uint8_t*)buffer_to_log, 18);
		
		myDevice.isBusy = IDLE; //Set idle status for device
		
		return true;
}

/*!
 * @brief erase full chip
 *
 * @param[in] none
 */
void W25Q16_Erase_Chip(void)
{
		/*!
		****************Step to follow**************************************************************
		1. Wait end busy status
		2. Set busy status for device
		3. Write enable
		4. Enable SPI
		5. Send CHIP_ERASE instruction
		6. Disable SPI
		7. Wait for end operation
		8. Set idle status for device
		******************************************************************************
		*/
		while(myDevice.isBusy == BUSY); //Wait end busy status
		myDevice.isBusy  = BUSY;	//Set busy status for device
		W25Q16_Enable_Write();	//Write enable
		W25Q16_CS_EN; //Enable SPI
		W25Q16_Trans_Recv(CHIP_ERASE); //Send CHIP_ERASE instruction
		W25Q16_CS_DIS; //Disable SPI
		W25Q16_Wait_EndBusy(); //Wait for end operation
		
		Log_Info((uint8_t*)"Erase chip success!!\n", 21);
		HAL_Delay(10);
		
		myDevice.isBusy = IDLE; //Set idle status for device	
}

/*!
 * @brief erase sector
 *
 * @param[in] idSector: id of sector to erase (0,1,...511)
 */
void W25Q16_Erase_Sector(uint16_t idSector)
{
		/*!
		****************Step to follow**************************************************************
		1. Wait end busy status
		2. Set busy status for device
		3. Wait for end operation
		4. Caculate real address of sector 
		5. Write enable
		6. Enable SPI
		7. Send SECTOR_ERASE instruction, send address of sector
		8. Disable SPI
		9. Wait for end operation
		10. Set idle status for device
		******************************************************************************
		*/
		uint32_t addrSector = 0;
	
		while(myDevice.isBusy == BUSY);
		myDevice.isBusy = BUSY;
		W25Q16_Wait_EndBusy();
		addrSector = idSector * myDevice.sizeSector;
		W25Q16_Enable_Write();	//Write enable
		W25Q16_CS_EN;
		W25Q16_Trans_Recv(SECTOR_ERASE);
		W25Q16_Trans_Recv((addrSector  & 0xFF0000) >> 16);
		W25Q16_Trans_Recv((addrSector  & 0xFF00) >> 8);
		W25Q16_Trans_Recv((addrSector  & 0xFF) >> 0);
		W25Q16_CS_DIS;
		W25Q16_Wait_EndBusy();
		
		Log_Info((uint8_t*)"Erase sector success!!\n", 23);
		HAL_Delay(1);
		
		myDevice.isBusy = IDLE;
}

/*!
 * @brief erase block
 *
 * @param[in] idBlock: id of sector to erase (0,1,...31)
 */
void W25Q16_Erase_Block(uint8_t idBlock)
{
		/*!
		****************Step to follow**************************************************************
		1. Wait end busy status
		2. Set busy status for device
		3. Wait for end operation
		4. Caculate real address of sector 
		5. Write enable
		6. Enable SPI
		7. Send BLOCK_ERASE instruction, send address of sector
		8. Disable SPI
		9. Wait for end operation
		10. Set idle status for device
		******************************************************************************
		*/
		uint32_t addrBlock = 0;
	
		while(myDevice.isBusy == BUSY);
		myDevice.isBusy = BUSY;
		W25Q16_Wait_EndBusy();
		addrBlock = idBlock * myDevice.sizeBlock;
		W25Q16_Enable_Write();	//Write enable
		W25Q16_CS_EN;
		W25Q16_Trans_Recv(BLOCK_ERASE);
		W25Q16_Trans_Recv((addrBlock  & 0xFF0000) >> 16);
		W25Q16_Trans_Recv((addrBlock  & 0xFF00) >> 8);
		W25Q16_Trans_Recv((addrBlock  & 0xFF) >> 0);
		W25Q16_CS_DIS;
		W25Q16_Wait_EndBusy();
		
		Log_Info((uint8_t*)"Erase block success!!\n", 22);
		HAL_Delay(1);
		
		myDevice.isBusy = IDLE;	
}

/*!
 * @brief convert id of page to id of sector
 *
 * @param[in] idPage: id of page to convert (0,1,...8191)
 */
uint16_t W25Q16_Page2Sector(uint8_t idPage)
{
		return ((idPage * myDevice.sizePage) / myDevice.sizeSector);
}

/*!
 * @brief convert id of page to id of block
 *
 * @param[in] idPage: id of page to convert (0,1,...8191)
 */
uint16_t W25Q16_Page2Block(uint8_t idPage)
{
		return ((idPage * myDevice.sizePage) / myDevice.sizeBlock);
}

/*!
 * @brief convert id of sector to id of block
 *
 * @param[in] idSector: id of page to convert (0,1,...511)
 */
uint16_t W25Q16_Sector2Block(uint8_t idSector)
{
		return ((idSector * myDevice.sizeSector) / myDevice.sizeBlock);
}

/*!
 * @brief convert id of sector to id of page
 *
 * @param[in] idSector: id of page to convert (0,1,...511)
 */
uint16_t W25Q16_Sector2Page(uint8_t idSector)
{
		return ((idSector * myDevice.sizeSector) / myDevice.sizePage);
}

/*!
 * @brief convert id of block to id of page
 *
 * @param[in] idBlock: id of page to convert (0,1,...31)
 */
uint16_t W25Q16_Block2Page(uint8_t idBlock)
{
		return ((idBlock * myDevice.sizeBlock) / myDevice.sizePage);
}

/*!
 * @brief write a byte into flash
 *
 * @param[in] data: value to write into flash
 * @param[in] addrToWrite: address of byte to write
 */
void W25Q16_WriteByte(uint8_t data,
											uint32_t addrToWrite)
{
		/*!
		****************Step to follow**************************************************************
		1. Wait end busy status
		2. Set busy status for device
		3. Wait for end operation
		4. Write enable
		5. Enable SPI
		6. Send PAGE_PROGRAM instruction, send address of sector, send data via SPI
		7. Disable SPI
		8. Wait for end operation
		9. Set idle status for device
		******************************************************************************
		*/
		while(myDevice.isBusy == BUSY);
		myDevice.isBusy  = BUSY;
		W25Q16_Wait_EndBusy();
		W25Q16_Enable_Write();	//Write enable
		W25Q16_CS_EN;
		W25Q16_Trans_Recv(PAGE_PROGRAM);
		W25Q16_Trans_Recv((addrToWrite  & 0xFF0000) >> 16);
		W25Q16_Trans_Recv((addrToWrite  & 0xFF00) >> 8);
		W25Q16_Trans_Recv((addrToWrite  & 0xFF) >> 0);
		W25Q16_Trans_Recv(data);
		W25Q16_CS_DIS;
		W25Q16_Wait_EndBusy();
		
		Log_Info((uint8_t*)"Write a byte SUCCESS\n", 21);
		myDevice.isBusy = IDLE;
}

/*!
 * @brief write data into a page
 *
 * @param[in] pData: pointer to data buffer
 * @param[in] idPage: id of pages (0,1,...8191)
 * @param[in] number: number of datas to write 
 * @param[in] offSet: offset value of start address (byte)
 */
void W25Q16_WritePage(uint8_t* pData,
											uint16_t idPage, uint8_t offSet, uint16_t number)
{
		/*!
		****************Step to follow**************************************************************
		1. Wait end busy status
		2. Set busy status for device
		3. Wait for end operation, calculate real address of Page
		4. Write enable
		5. Enable SPI
		6. Send PAGE_PROGRAM instruction, send address of sector, send data via SPI
		7. Disable SPI
		8. Wait for end operation
		9. Set idle status for device
		******************************************************************************
		*/
		uint32_t addrPage = 0;
		while(myDevice.isBusy == BUSY);
		myDevice.isBusy  = BUSY;
		if((number + offSet) > myDevice.sizePage) number = myDevice.sizePage - offSet;
	
		W25Q16_Wait_EndBusy();
		addrPage = (idPage * myDevice.sizePage) + offSet;
		W25Q16_Enable_Write();	//Write enable
		W25Q16_CS_EN;
		W25Q16_Trans_Recv(PAGE_PROGRAM);
		W25Q16_Trans_Recv((addrPage  & 0xFF0000) >> 16);
		W25Q16_Trans_Recv((addrPage  & 0xFF00) >> 8);
		W25Q16_Trans_Recv((addrPage  & 0xFF) >> 0);
		HAL_SPI_Transmit(&W25Q16_SPI, pData, number, 100);
		W25Q16_CS_DIS;
		W25Q16_Wait_EndBusy();
		
		Log_Info((uint8_t*)"Write page SUCCESS\n", 21);
		myDevice.isBusy = IDLE;
}

/*!
 * @brief read a byte from flash
 *
 * @param[in] pBuffer: pointer to buffer to contain from flash
 * @param[in] addrToRead: address of byte to read
 */
void W25Q16_ReadByte(uint8_t* pBuffer,
										 uint32_t addrToRead)
{
		/*!
		****************Step to follow**************************************************************
		1. Wait end busy status
		2. Set busy status for device
		3. Wait for end operation
		5. Enable SPI
		6. Send FAST_READ instruction, send address of memory, send a dummy byte, receive data
		7. Disable SPI
		8. Wait for end operation
		9. Set idle status for device
		******************************************************************************
		*/
		char buffet_to_log[20];
		while(myDevice.isBusy == BUSY);
		myDevice.isBusy  = BUSY;
		W25Q16_Wait_EndBusy();
		W25Q16_CS_EN;
		W25Q16_Trans_Recv(FAST_READ);
		W25Q16_Trans_Recv((addrToRead & 0xFF0000) >> 16);
		W25Q16_Trans_Recv((addrToRead & 0xFF00) >> 8);
		W25Q16_Trans_Recv((addrToRead & 0xFF) >> 0);
		W25Q16_Trans_Recv(DUMMY_BYTE);
		//*pBuffer = W25Q16_Trans_Recv(DUMMY_BYTE);
		HAL_SPI_Receive(&W25Q16_SPI, pBuffer, 1, 100);
		W25Q16_CS_DIS;
		W25Q16_Wait_EndBusy();
	
//		sprintf(buffet_to_log, "Read a byte %02x\n", *pBuffer);
//		Log_Info((uint8_t*)buffet_to_log, 15);
		
		myDevice.isBusy  =  IDLE;
}

/*!
 * @brief read some bytes from flash
 *
 * @param[in] pBuffer: pointer to buffer to contain from flash
 * @param[in] addrToRead: address of byte to write
 * @param[in] number: number of bytes to read
 */
void W25Q16_ReadSomeBytes(uint8_t* pBuffer,
													uint32_t addrToRead, uint32_t number)
{
		/*!
		****************Step to follow**************************************************************
		1. Wait end busy status
		2. Set busy status for device
		3. Wait for end operation
		5. Enable SPI
		6. Send FAST_READ instruction, send address of memory, send a dummy byte, receive data
		7. Disable SPI
		8. Wait for end operation
		9. Set idle status for device
		******************************************************************************
		*/
//		char buffer_to_log[30];
		while(myDevice.isBusy == BUSY);
		myDevice.isBusy  = BUSY;
		W25Q16_Wait_EndBusy();
		W25Q16_CS_EN;
		W25Q16_Trans_Recv(FAST_READ);
		W25Q16_Trans_Recv((addrToRead & 0xFF0000) >> 16);
		W25Q16_Trans_Recv((addrToRead & 0xFF00) >> 8);
		W25Q16_Trans_Recv((addrToRead & 0xFF) >> 0);
		//W25Q16_Trans_Recv(DUMMY_BYTE);
		W25Q16_Trans_Recv(0);
		HAL_SPI_Receive(&W25Q16_SPI, pBuffer, number, 1000);
		
		W25Q16_CS_DIS;
		#if (DEBUG == 1)
		for(uint32_t i = 0; i < number; i++)
		{
				sprintf(buffer_to_log, "Read a byte %02x\n", pBuffer[i]);
				Log_Info((uint8_t*)buffer_to_log, 15);
		}
		#endif
		
		myDevice.isBusy = IDLE;
}

/*!
 * @brief read some bytes of page from flash
 *
 * @param[in] pBuffer: pointer to buffer to contain from flash
 * @param[in] idPage: id of page to read data (0,1,...8191)
 * @param[in] number: number of bytes to read
 * @param[in] offSet: offset value of start address (byte)
 */
void W25Q16_ReadPage(uint8_t* pBuffer,
										 uint32_t idPage, uint8_t offSet, uint16_t number)
{
		/*!
		****************Step to follow**************************************************************
		1. Wait end busy status
		2. Set busy status for device
		3. Wait for end operation, calculate real address of page
		5. Enable SPI
		6. Send FAST_READ instruction, send address of memory, send a dummy byte, receive data
		7. Disable SPI
		8. Wait for end operation
		9. Set idle status for device
		******************************************************************************
		*/
//		char buffer_to_log[30];
		uint32_t addrPage = 0;
		while(myDevice.isBusy == BUSY);
		myDevice.isBusy  = BUSY;
		W25Q16_Wait_EndBusy();
		if(number > myDevice.sizePage) number = myDevice.sizePage;
		if((number + offSet > myDevice.sizePage)) number = myDevice.sizePage - offSet;
		addrPage = idPage * myDevice.sizePage + offSet;
		
		W25Q16_CS_EN;
		W25Q16_Trans_Recv(FAST_READ);
		W25Q16_Trans_Recv((addrPage & 0xFF0000) >> 16);
		W25Q16_Trans_Recv((addrPage & 0xFF00) >> 8);
		W25Q16_Trans_Recv((addrPage & 0xFF) >> 0);	
		W25Q16_Trans_Recv(DUMMY_BYTE);
		HAL_SPI_Receive(&W25Q16_SPI, pBuffer, number, 1000);
		W25Q16_CS_DIS;
		
		#if (DEBUG == 1)
		for(uint32_t i = 0; i < number; i++)
		{
				sprintf(buffer_to_log, "Read a byte %02x\n", pBuffer[i]);
				Log_Info((uint8_t*)buffer_to_log, 15);
		}
		#endif
		myDevice.isBusy = IDLE;
}

/*Functions relate convert float to string*/

/*!
 * @brief reverse a string
 *
 * @param[in] str: pointer to input string
 * @param[in] len: length of string
 */
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

/*!
 * @brief convert a int number to string
 *
 * @param[in] inputNum: input number type int
 * @param[in] str: buffer input to get string
 * @param[in] numDigit: number of digits want to output
 * @retval: number of digits output
 */
uint8_t intToStr(uint32_t inputNum, char* str, uint8_t numDigit)
{
    int i = 0;
    while (inputNum) {
        str[i++] = (inputNum % 10) + '0';
        inputNum = inputNum / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < numDigit)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

/*!
 * @brief Converts a floating-point/double number to a string
 *
 * @param[in] number: input number type double/float
 * @param[in] res: restrict to get string
 * @param[in] afterPoint: number of digits want to after point
 * @retval none
 */
void ftoa(double number, char* res, uint8_t afterPoint)
{
	 // Extract integer part
    uint32_t ipart = (uint32_t)number;
    // Extract floating part
    double fpart = number - (double)ipart;
    // convert integer part to string
    uint8_t i = intToStr(ipart, res, 0);
    // check for display option after point
    if (afterPoint != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterPoint);

        intToStr((uint32_t)fpart, res + i + 1, afterPoint);
    }
}

/*Functions relate convert uint16_t to string*/

/*!
 * @brief Converts a number size u16 to string
 *
 * @param[in] res: restrict to get string
 * @param[in] inputValue: input number
 * @retval none
 */
void u16_to_String(char* res, uint16_t inputValue)
{
    uint8_t i = 0, count = 0;
    uint16_t temp = inputValue;
	  if(temp == 0) *res = 0x30;
    while(temp != 0)
    {
        temp = temp /10;
        count++;
    }
    i = count - 1;
    while(inputValue != 0)
    {
        temp = inputValue % 10;
        *(res + i) = temp + 0x30;
        i--;
        inputValue /= 10;
    }
}


/*EOF*/
