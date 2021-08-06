#ifndef __FSL_SOFT_UART_H
#define __FSL_SOFT_UART_H

#include "stm32f4xx_hal_uart_emul.h"
#include "stm32f4xx_hal_def.h" 

void Log_Info(uint8_t* inputStr,  uint8_t sizeStr);
void UartEmul_Init(void);
//void HAL_UART_Emul_RxCpltCallback(UART_Emul_HandleTypeDef *huart);
//void HAL_UART_Emul_ErrorCallback(UART_Emul_HandleTypeDef *UartEmulHandle);
void Trans_Data(UART_Emul_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void Recv_Data(void);

#endif
