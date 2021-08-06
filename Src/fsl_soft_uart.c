#include "fsl_soft_uart.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart_emul.h"

extern UART_HandleTypeDef huart1;
extern UART_Emul_HandleTypeDef UartEmulHandle;

/**
  * @brief  Sends an amount of data in blocking mode.
	*
  * @param  sizeStr  Amount of data elements to be sent
  * @param  inputStr pointer to data elements to be sent
  * @retval none
  */
void Log_Info(uint8_t* inputStr,  uint8_t sizeStr)
{
		if(inputStr != NULL)
		{	
		    HAL_UART_Transmit(&huart1, inputStr, sizeStr, 1000);
		}
		else 
		{
				HAL_UART_Transmit(&huart1, (uint8_t*)"Data Log NULL\n", 14, 1000);
		}
}

void UartEmul_Init(void)
{
		UartEmulHandle.Init.Mode        = UART_EMUL_MODE_TX_RX;
		UartEmulHandle.Init.BaudRate    = 115200;
		UartEmulHandle.Init.StopBits    = UART_EMUL_STOPBITS_1;
		UartEmulHandle.Init.Parity      = UART_EMUL_PARITY_NONE;
		UartEmulHandle.Init.WordLength  = UART_EMUL_WORDLENGTH_8B;
		
		if (HAL_UART_Emul_Init(&UartEmulHandle) != HAL_OK)
		{
			Error_Handler();
		}
}

void HAL_UART_Emul_RxCpltCallback(UART_Emul_HandleTypeDef *huart)
{
		g_recv_buff[g_count++] = g_recv_byte;
		Log_Info((uint8_t*)"RX complete\n", 12);
}

void HAL_UART_Emul_ErrorCallback(UART_Emul_HandleTypeDef *UartEmulHandle)
{
		Log_Info((uint8_t*)"BUG\n", 4);
}

void Trans_Data(UART_Emul_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
		UartEmul_Init();
		if (HAL_UART_Emul_Transmit_DMA(&UartEmulHandle, (uint8_t*) aTxBuffer1, 19) != HAL_OK)
		{
			Error_Handler();
		}
		while (__HAL_UART_EMUL_GET_FLAG(&UartEmulHandle, UART_EMUL_FLAG_TC) != SET){};
}

void Recv_Data(void)
{
		UartEmul_Init();
		if (HAL_UART_Emul_Receive_DMA(&UartEmulHandle, &g_recv_byte, 1) != HAL_OK)
		{
				Error_Handler();
		}
		while (__HAL_UART_EMUL_GET_FLAG(&UartEmulHandle, UART_EMUL_FLAG_RC) != SET){}
		if (g_recv_byte == '\r')	
		{
				g_recv_buff[--g_count] = '\0';	
		}
		else if((g_check_lf == LF_FALSE) && g_recv_byte == '\n')
		{
				g_recv_buff[--g_count] = '\0';
				g_check_lf = LF_TRUE;	//completed first <CR><LF> of response
		}
		else if ((g_check_lf == LF_TRUE) && (g_recv_byte == '\n')) 
		{
				g_recv_buff[--g_count] = '\0';
				g_isDone = RX_TRUE;
		}
		else Log_Info(&g_recv_byte, 1);
		__HAL_UART_EMUL_CLEAR_FLAG(&UartEmulHandle, UART_EMUL_FLAG_RC);
}