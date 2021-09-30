#include "MiddleWare.h"

/*global variables for LTE module*/
extern uint8_t g_recv_byte;
extern uint8_t g_isDone, g_check_ok, g_check_end;
extern uint16_t	g_count, g_count_temp;
extern uint8_t g_recv_buff[MAX_SIZE_BUFF], g_buff_temp[MAX_SIZE_BUFF];
extern uint8_t g_buffer_log[30];
extern uint8_t g_id_msg, g_size_sms;
extern uint32_t g_timeNow;

/*global variables for Flash module*/
extern uint8_t 		g_data_read;

/*global variables for Bootloader*/
extern uint32_t 		g_user_address;
extern uint8_t 		  g_buff_hex_line[50];
extern uint32_t 		g_buff_data_boot[10000];
extern uint32_t 		g_count_data_boot;
extern uint32_t 		g_start_line_addr;
extern HexTypedef_t aLineHex;

/**
  * @brief  Get a line in hex file, copy to buffer
	* @param  targetBuff:  buff target to pass data
	* @param  addrStart: address of start data in external flash
  * @retval address of new start data in external flash(stop data + 1)
  */
uint32_t Get_Hex_Line(uint8_t* targetBuff, uint32_t addrStart)
{
		uint32_t newStart = addrStart;
		g_data_read = '\0';	//reset 'g_data_read'
		while(g_data_read != '\n')	//until end line
		{
				W25Q16_ReadByte(&g_data_read, newStart);
				targetBuff[newStart - addrStart] = g_data_read;	//copy data to target buffer
				newStart++; //increase new start index
		}
		return newStart;
}

/**
  * @brief  Convert ascii to decimal
	* @param  character:  character input
  * @retval value in decimal
  */
uint8_t Ascii_to_Dec(uint8_t character)
{
		if(('0' <= character) && (character <= '9')) return (character - '0');
		if(('A' <= character) && (character <= 'F')) return (character - 'A' + 10);
		else return (character - 'a' + 10); //if(('a' <= character) && (character <= 'f')) 
}

/**
  * @brief  Write a buffer to internal flash
	* @param  none
  * @retval none
  */
void Write_Flash(void)
{
		Log_Info((uint8_t*)"\r\nStartFlash\r\n", 14);
		HAL_FLASH_Unlock();
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
							FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
		//erase sector of app
		FLASH_Erase_Sector(APP_SECTOR, FLASH_VOLTAGE_RANGE_3);
	
		for(uint32_t i = 0; i < g_count_data_boot; i++)
		{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, g_user_address, g_buff_data_boot[i]);
				g_user_address += 4;
		}
		HAL_FLASH_Lock();
		Log_Info((uint8_t*)"\r\nEndFlash\r\n", 12);
}

/**
  * @brief  Process a line in hex file
	* @param  line_hex_buff:  buff include data of a hex line
	* @param  addrStart: address of start data in external flash
  * @retval 0(check sum false or not flash data) or 1(check sum true)
  */
uint8_t Process_Hex_Line(uint8_t* line_hex_buff)
{

		uint8_t numberOfData = 0;
		uint16_t check = 0;
		/*
			character 0 is ':', ignore
			character 1,2 are byte count
			charecter 3,4,5,6 are address
			charecter 7,8 are record type
			2 last charecters are check sum
			the rest is data
		*/
	
		//convert byte count, address, record type, check sum  => to DEC
		aLineHex.byteCount = Ascii_to_Dec(line_hex_buff[1]) * 16 + Ascii_to_Dec(line_hex_buff[2]);
		aLineHex.addRess = Ascii_to_Dec(line_hex_buff[3]) * pow(16, 3) + Ascii_to_Dec(line_hex_buff[4]) * pow(16, 2)
											 + Ascii_to_Dec(line_hex_buff[5]) * 16 + Ascii_to_Dec(line_hex_buff[6]);
		aLineHex.recordType = Ascii_to_Dec(line_hex_buff[7]) * 16 + Ascii_to_Dec(line_hex_buff[8]);
		numberOfData = 2 * aLineHex.byteCount;
		aLineHex.checkSum = Ascii_to_Dec(line_hex_buff[9+numberOfData]) * 16 + Ascii_to_Dec(line_hex_buff[10+numberOfData]);
		//get datas of a line
		if((numberOfData > 0) && (aLineHex.recordType == 0x00))
		{
				uint8_t temp_data[32] = {0};	
				for(uint8_t i = 0; i < numberOfData; i++)
				{
					//data from 9,10...,40 on a line
					temp_data[i] = Ascii_to_Dec(line_hex_buff[9+i]);
				}
			
				//convert data => u32bit, eg: 0,1,2,3,4,5,6,7 => 67452301
				for(uint8_t i = 0; i < 4; i++)
				{
					aLineHex.u32data[i] = 0;	
					aLineHex.u32data[i] = ((temp_data[8*i+0] << 4) | temp_data[8*i+1]) << 0; 
					aLineHex.u32data[i] |= ((temp_data[8*i+2] << 4) | temp_data[8*i+3]) << 8;
					aLineHex.u32data[i] |= ((temp_data[8*i+4] << 4) | temp_data[8*i+5]) << 16;
					aLineHex.u32data[i] |= ((temp_data[8*i+6] << 4) | temp_data[8*i+7]) << 24;
				}			
				//check the checksum
				check = aLineHex.byteCount + aLineHex.recordType + (0xFF &(aLineHex.addRess >> 8)) + (0xFF & aLineHex.addRess);
				for(uint8_t i = 0; i < 4; i++)
				{
						for(uint8_t j = 0; j < 4; j++)					
						{
								check += ( 0xFF & (aLineHex.u32data[i] >> (8*j)) );
						}
				}
				if( (0xFF & ((~check) + 1)) == aLineHex.checkSum) 
				{
						Log_Info((uint8_t*)"CheckSum True!\r\n", 16);
						for(uint8_t i = 0; i < 4; i++)
						{
								g_buff_data_boot[g_count_data_boot] = aLineHex.u32data[i];
								g_count_data_boot++;
						}
						return 1;
				}
				else 
				{
						Log_Info((uint8_t*)"CheckSum False\r\n", 16);
						return 0;
				}
		}		
		Log_Info((uint8_t*)"Data not to Flash\r\n", 19);
		return 0; //not flash data
}

/**
  * @brief  Boot function, jump to new firmware 
	* @param  none
  * @retval none
  */
void Boot(void)
{
  HAL_RCC_DeInit();
	HAL_DeInit();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;

	/* Clear Pending In terrupt Request, turn  off System Tick*/
	SCB->SHCSR &= ~( SCB_SHCSR_USGFAULTENA_Msk |\
									SCB_SHCSR_BUSFAULTENA_Msk |\
									SCB_SHCSR_MEMFAULTENA_Msk ) ;
	
	__disable_irq ();

	/* Set Main Stack Pointer*/
	__set_MSP(*((volatile uint32_t*) APP_ADDRESS));
	/* Set Program Counter to Blink LED Apptication Address*/
	void (*jump)(void) =  (void(*)(void)) (*((volatile uint32_t *) (APP_ADDRESS + 4)));
  jump();
	while(1);
}
/*end of file*/
