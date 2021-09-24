#include "soft_uart.h"
#include <stdio.h>


/*Global Variables*/
extern uint8_t g_recv_byte;
extern uint8_t g_isDone, g_check_ok, g_check_end;
extern uint16_t	g_count, g_count_temp;
extern uint8_t g_recv_buff[MAX_SIZE_BUFF], g_buff_temp[MAX_SIZE_BUFF];
extern uint8_t g_buffer_log[30];
extern uint32_t g_timeNow;

/**
  * @brief  Initializes the UART Emulation 
  * @param  huart: UART Emulation Handle
  * @retval None
  */
void UartEmul_Init(UART_Emul_HandleTypeDef *huart)
{
		huart->Init.Mode        = UART_EMUL_MODE_TX_RX;
		huart->Init.BaudRate    = 4800; //9600; //115200;
		huart->Init.StopBits    = UART_EMUL_STOPBITS_1;
		huart->Init.Parity      = UART_EMUL_PARITY_NONE;
		huart->Init.WordLength  = UART_EMUL_WORDLENGTH_8B;
		
		if (HAL_UART_Emul_Init(huart) != HAL_OK)
		{
				HAL_Delay(500);
		}
}

/**
  * @brief  Initializes the UART Emulation with baudrate 115200
  * @param  huart: UART Emulation Handle
  * @retval None
  */
void UartEmul_Init_115200(UART_Emul_HandleTypeDef *huart)
{
		huart->Init.Mode        = UART_EMUL_MODE_TX_RX;
		huart->Init.BaudRate    = 115200; //9600; //115200;
		huart->Init.StopBits    = UART_EMUL_STOPBITS_1;
		huart->Init.Parity      = UART_EMUL_PARITY_NONE;
		huart->Init.WordLength  = UART_EMUL_WORDLENGTH_8B;
		
		if (HAL_UART_Emul_Init(huart) != HAL_OK)
		{
				HAL_Delay(500);
		}
}

/**
  * @brief  Receive data complete callback
  * @param  huart: UART Emulation Handle
  * @retval None
  */
void HAL_UART_Emul_RxCpltCallback(UART_Emul_HandleTypeDef *huart)
{
		g_recv_buff[g_count++] = g_recv_byte;
}
/**
  * @brief  UART Emul error callback
  * @param  huart: UART Emulation Handle
  * @retval None
  */
void HAL_UART_Emul_ErrorCallback(UART_Emul_HandleTypeDef *UartEmulHandle)
{
		Log_Info((uint8_t*)"BUG\n", 4);
}

/**
  * @brief  Transmit a string data
  * @param  huart: UART Emulation Handle
	* @param  pData: pointer to data string
	* @param  Size: size of data
  * @retval None
  */
void Trans_Data(UART_Emul_HandleTypeDef * huart, uint8_t *pData, uint16_t Size)
{
		UartEmul_Init(huart);
		HAL_Delay(10);
		UartEmul_Init(huart);
		HAL_Delay(10);
		if (HAL_UART_Emul_Transmit_DMA(huart, pData, Size) != HAL_OK)
		{
				Log_Info((uint8_t*)"Error_Handler\n", 14);
		}
		while (__HAL_UART_EMUL_GET_FLAG(huart, UART_EMUL_FLAG_TC) != SET){};
}

/**
  * @brief  Transmit a string data with baudrate 115200
  * @param  huart: UART Emulation Handle
	* @param  pData: pointer to data string
	* @param  Size: size of data
  * @retval None
  */
void Trans_Data_115200(UART_Emul_HandleTypeDef * huart, uint8_t *pData, uint16_t Size)
{
		UartEmul_Init_115200(huart);
		HAL_Delay(10);
		UartEmul_Init_115200(huart);
		HAL_Delay(10);
		if (HAL_UART_Emul_Transmit_DMA(huart, pData, Size) != HAL_OK)
		{
				Log_Info((uint8_t*)"Error_Handler\n", 14);
		}
		while (__HAL_UART_EMUL_GET_FLAG(huart, UART_EMUL_FLAG_TC) != SET){};
}

/**
  * @brief  Receive a byte data, copy to 'g_recv_byte' variable
  * @param  huart: UART Emulation Handle
	* @param  timeNow: tick value of moment call this function
	* @param  timeOut: time to wait response
  * @retval none
  */
void Recv_Data(UART_Emul_HandleTypeDef * huart, uint32_t timeNow, uint32_t timeOut)
{		
//		UartEmul_Init(huart);
		if (HAL_UART_Emul_Receive_DMA(huart, &g_recv_byte, 1) != HAL_OK)
		{
//				HAL_Delay(500);
//				Log_Info((uint8_t*)"Error_Handler\n", 14);
		}
		while (__HAL_UART_EMUL_GET_FLAG(huart, UART_EMUL_FLAG_RC) != SET)
		{
				if ((HAL_GetTick() - timeNow) > timeOut)
				{
						__HAL_UART_EMUL_SET_FLAG(huart, UART_EMUL_FLAG_RC);
						g_isDone = RX_TRUE;	
				}
		}
		__HAL_UART_EMUL_CLEAR_FLAG(huart, UART_EMUL_FLAG_RC);
}

/**
  * @brief  Compare 2 strings 
  * @param  str1, str2: pointer to 2 addresses of strings
	* @param  len: length of datas to compare
  * @retval true(1) or false(0)
  */
uint8_t Compare_Str(uint8_t* str1, uint8_t* str2, uint8_t len)
{
		while(len)
		{
				if(*str1++ != *str2++)
				{
						return 0;
				}
				len--;
		}
		return 1;
}

/**
  * @brief  Compare string with "\r\nOK\r\n" 
  * @param  inputStr: pointer to addres of string input
  * @retval true(1) or false(0)
  */
uint8_t Confirm_OK(uint8_t* inputStr)
{
		if(Compare_Str(inputStr, (uint8_t*)"\r\nOK\r\n", 6)) return 1;
		return 0;
}

/**
  * @brief  Check response from UART, if MQTT Status is close
	* @param  inputStr: pointer to input string
  * @retval true(1) or false(0)
  */
uint8_t Check_MQTT(uint8_t* inputStr)
{
		if(Compare_Str(inputStr, (uint8_t*)"\r\n+QMTSTAT: 0,1\r\n", 17)) return 1;
		return 0;
}

/**
  * @brief  Receive a string 
  * @param  huart: UART Emulation Handle
	* @param  timeOut: time to wait response
  * @retval RESPONSE_OK, RESPONSE_ERR or RESPONSE_NEW_MSG
  */
response_t Recv_Response(UART_Emul_HandleTypeDef *huart, uint32_t timeOut)
{
			response_t retValue = RESPONSE_ERR;
			while(!g_isDone)
			{		
					g_timeNow = HAL_GetTick();		
					Recv_Data(huart, g_timeNow, timeOut);
			}
//			Log_Info(g_recv_buff, g_count);
			g_isDone = RX_FALSE;
			
			if(Confirm_OK(&g_recv_buff[g_count-6])) //if response stop by "\r\nOK\r\n"
			{
				retValue = RESPONSE_OK;
			}
			if(Check_New_Msg(g_recv_buff))	//if response start by "\r\n+CTMI"
			{
				retValue = RESPONSE_NEW_MSG;
			}
			if(Check_MQTT(&g_recv_buff[g_count-17]))
			{
					retValue = RESPONSE_MQTT_CLOSE;
			}
			g_count_temp = g_count;	//pass value of 'g_count' into 'g_count_temp' to use in user file
			g_count = 0;
			return retValue;
}

/**
	*	@note  	Use like "Recv_Data" but for blocking mode (Recv_Response_Continue) or after Transmit short data
  * @brief  Receive a byte data, copy to 'g_recv_byte' variable 
  * @param  huart: UART Emulation Handle
	* @param  timeNow: tick value of moment call this function
	* @param  timeOut: time to wait response
  * @retval none
  */
void Recv_Data_Continue(UART_Emul_HandleTypeDef * huart, uint32_t timeNow, uint32_t timeOut)
{		
		UartEmul_Init(huart);
		if (HAL_UART_Emul_Receive_DMA(huart, &g_recv_byte, 1) != HAL_OK)
		{
				HAL_Delay(500);
				Log_Info((uint8_t*)"Error_Handler\n", 14);
		}
		while (__HAL_UART_EMUL_GET_FLAG(huart, UART_EMUL_FLAG_RC) != SET)
		{
				if ((HAL_GetTick() - timeNow) > timeOut)
				{
						__HAL_UART_EMUL_SET_FLAG(huart, UART_EMUL_FLAG_RC);
						g_isDone = RX_TRUE;	
				}
		}
		__HAL_UART_EMUL_CLEAR_FLAG(huart, UART_EMUL_FLAG_RC);
}

/**
	* @note		Use like "Recv_Response" but for blocking mode or after Transmit short data
  * @brief  Receive a string 
  * @param  huart: UART Emulation Handle
	* @param  timeOut: time to wait response
  * @retval RESPONSE_OK, RESPONSE_ERR or RESPONSE_NEW_MSG
  */
response_t Recv_Response_Continue(UART_Emul_HandleTypeDef *huart, uint32_t timeOut)
{
			response_t retValue = RESPONSE_ERR;
			while(!g_isDone)
			{		
					g_timeNow = HAL_GetTick();		
					Recv_Data_Continue(huart, g_timeNow, timeOut);
			}
			Log_Info(g_recv_buff, g_count);
			g_isDone = RX_FALSE;
			
			if(Confirm_OK(&g_recv_buff[g_count-6])) //if response stop by "\r\nOK\r\n"
			{
					retValue = RESPONSE_OK;
			}
			if(Check_New_Msg(g_recv_buff))	//if response start by "\r\n+CTMI"
			{
					retValue = RESPONSE_NEW_MSG;
			}
			if(Check_MQTT(&g_recv_buff[g_count-17]))
			{
					retValue = RESPONSE_MQTT_CLOSE;
			}
			g_count_temp = g_count;	//pass value of 'g_count' into 'g_count_temp' to use in user file
			g_count = 0;
			return retValue;
}

/**
  * @brief  Copy data from source str and pass into target str
	* @param  strTarget: pointer to target str
	* @param  strSource: pointer to source str
	* @param  idStartSource: id start copy of source str
	* @param  idStopSource: id stop copy of source str
  * @retval none
  */
void Get_Paragraph(uint8_t* strTarget, uint8_t* strSource, uint16_t idStartSource, uint16_t idStopSource)
{	
		uint16_t number = idStopSource - idStartSource + 1;
		for(uint16_t i = 0; i < number; i++)
		{
				*(strTarget + i) = *(strSource + idStartSource + i);
		}
}

/**
  * @brief  Check response from UART, is a new message or not
	* @param  inputStr: pointer to input string
  * @retval true(1) or false(0)
  */
uint8_t Check_New_Msg(uint8_t* inputStr)
{
    Get_Paragraph(g_buff_temp, inputStr, 0, 6);
    if(Compare_Str(g_buff_temp, (uint8_t*)"\r\n+CMTI", 7)) return 1;
        else return 0;
}

/**
  * @brief  Get id of message
	* @param  inputStr: pointer to input string
  * @retval id of message
  */
uint8_t Get_ID_Msg(uint8_t* inputStr)
{
    uint8_t retValue = 0;
		if( (*(inputStr+15) >= 0x30) & (*(inputStr+15) <= 0x39) )    //if index > 10
		{
				retValue = (*(inputStr + 14) - 0x30) * 10 + (*(inputStr + 15) - 0x30);
				Log_Info((inputStr + 14), 1);	//test character of [14]  
		}
		else retValue = *(inputStr + 14) - 0x30;
    return retValue;
}

/**
  * @brief  Get data of message from response
	* @param  inputStr: pointer to input string
	* @param  sizeResponse: size of response include data sms
  * @retval size of data
  */
uint8_t Get_Data_Msg(uint8_t* inputStr, uint8_t* outputStr, uint8_t sizeResponse)
{
    uint8_t sizeDataMsg = 0, idStartMsg = sizeResponse - 9, idStopMsg = sizeResponse - 9;

		sprintf((char*)g_buffer_log, "\nID Stop = %d\n", idStopMsg);
    Log_Info(g_buffer_log, 14);
    while(*(inputStr + idStartMsg)!= '\n')
    {
        idStartMsg--;
    }
    idStartMsg++;
    sprintf((char*)g_buffer_log, "\nID Start= %d\n", idStartMsg);
    Log_Info(g_buffer_log, 15);
    sizeDataMsg = idStopMsg - idStartMsg + 1;
		
    sprintf((char*)g_buffer_log, "\nSizeMsg = %d\n", sizeDataMsg);
    Log_Info(g_buffer_log, 14);
		
    Get_Paragraph(outputStr, inputStr, idStartMsg, sizeResponse - 9);
    Log_Info(outputStr, sizeDataMsg);
		return sizeDataMsg;
}



/*End of file*/
