#ifndef __SOFT_UART_H
#define __SOFT_UART_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart_emul.h"

#define MAX_SIZE_BUFF 		256

/**
  * @brief  enum type for response from ME
  */
typedef enum 
{
		RESPONSE_ERR = 0U,		/*!<Response ERROR*/
		RESPONSE_OK,					/*!<Response OK*/
		RESPONSE_NEW_MSG,			/*!<New SMS notice*/
		RESPONSE_MQTT_CLOSE,	/*!<MQTT close notice notice*/
		RESPONSE_MQTT_EXIST
} response_t;

/**
  * @brief  Initializes the UART Emulation 
  * @param  huart: UART Emulation Handle
  * @retval None
  */
void UartEmul_Init(UART_Emul_HandleTypeDef *huart);

/**
  * @brief  Initializes the UART Emulation with baudrate 115200
  * @param  huart: UART Emulation Handle
  * @retval None
  */
void UartEmul_Init_115200(UART_Emul_HandleTypeDef *huart);
	
/**
  * @brief  Transmit a string data
  * @param  huart: UART Emulation Handle
	* @param  pData: pointer to data string
	* @param  Size: size of data
  * @retval None
  */
void Trans_Data(UART_Emul_HandleTypeDef * huart, uint8_t *pData, uint16_t Size);


/**
  * @brief  Transmit a string data with baudrate 115200
  * @param  huart: UART Emulation Handle
	* @param  pData: pointer to data string
	* @param  Size: size of data
  * @retval None
  */
void Trans_Data_115200(UART_Emul_HandleTypeDef * huart, uint8_t *pData, uint16_t Size);
	
/**
  * @brief  Receive a byte data, copy to 'g_recv_byte' variable
  * @param  huart: UART Emulation Handle
	* @param  timeNow: tick value of moment call this function
	* @param  timeOut: time to wait response
  * @retval none
  */
void Recv_Data(UART_Emul_HandleTypeDef * huart, uint32_t timeNow, uint32_t timeOut);

/**
  * @brief  Compare 2 strings 
  * @param  str1, str2: pointer to 2 addresses of strings
	* @param  len: length of datas to compare
  * @retval true(1) or false(0)
  */
uint8_t Compare_Str(uint8_t* str1, uint8_t* str2, uint8_t len);

/**
  * @brief  Compare string with "\r\nOK\r\n" 
  * @param  strInput: pointer to addres of string input
  * @retval true(1) or false(0)
  */
uint8_t Confirm_OK(uint8_t* inputStr);

/**
  * @brief  Check response from UART, if MQTT Status is close
	* @param  inputStr: pointer to input string
  * @retval true(1) or false(0)
  */
uint8_t Check_MQTT(uint8_t* inputStr);

/**
  * @brief  Receive a string 
  * @param  huart: UART Emulation Handle
	* @param  timeOut: time to wait response
  * @retval RESPONSE_OK or RESPONSE_ERR
  */
response_t Recv_Response(UART_Emul_HandleTypeDef *huart, uint32_t timeOut);

/**
  * @brief  Copy data from source str and pass into target str
	* @param  strTarget: pointer to target str
	* @param  strSource: pointer to source str
	* @param  idStartSource: id start copy of source str
	* @param  idStopSource: id stop copy of source str
  * @retval none
  */
void Get_Paragraph(uint8_t* strTarget, uint8_t* strSource, uint8_t idStartSource, uint8_t idStopSource);

/**
  * @brief  Check response from UART, is a new message or not
	* @param  inputStr: pointer to input string
  * @retval true(1) or false(0)
  */
uint8_t Check_New_Msg(uint8_t* inputStr);

/**
  * @brief  Get id of message
	* @param  inputStr: pointer to input string
  * @retval id of message
  */
uint8_t Get_ID_Msg(uint8_t* inputStr);

/**
  * @brief  Get data of message from response
	* @param  inputStr: pointer to input string
	* @param  outputStr: pointer to output string
	* @param  sizeResponse: size of response include data sms
  * @retval size of data
  */
uint8_t Get_Data_Msg(uint8_t* inputStr, uint8_t* outputStr, uint8_t sizeResponse);

/**
	*	@note  	Use like "Recv_Data" but for blocking mode (Recv_Response_Continue) or after Transmit short data
  * @brief  Receive a byte data, copy to 'g_recv_byte' variable 
  * @param  huart: UART Emulation Handle
	* @param  timeNow: tick value of moment call this function
	* @param  timeOut: time to wait response
  * @retval none
  */
void Recv_Data_Continue(UART_Emul_HandleTypeDef * huart, uint32_t timeNow, uint32_t timeOut);

/**
  * @brief  Receive a string 
  * @param  huart: UART Emulation Handle
	* @param  timeOut: time to wait response
  * @retval RESPONSE_OK, RESPONSE_ERR or RESPONSE_NEW_MSG
  */
response_t Recv_Response_Continue(UART_Emul_HandleTypeDef *huart, uint32_t timeOut);

#endif

/*End of file*/
