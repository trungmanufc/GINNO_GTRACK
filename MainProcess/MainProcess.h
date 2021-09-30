#ifndef _MAINPROCESS_H
#define _MAINPROCESS_H

#include "stm32f4xx_hal.h"
#include "soft_uart.h"
#include "stm32f4xx_hal_uart_emul.h"
#include "lte.h"
#include "w25q16.h"
#include "MiddleWare.h"

void Process_Main(void);
void Process_NewFirmware(void);

#endif

/*end of file*/
