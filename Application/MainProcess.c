/*
 * MainProcess.c
 *
 *  Created on: Sep 29, 2021
 *      Author: Admin
 */


#include"MainProcess.h"

void main_process(void)
{
		/* Initialize some peripherals conneted externally to the system */
	    System_Initialization();

	#if TEST_INIT == 1
	    HAL_Delay(500);

	    /* LTE module Init */
	    LTE_Init();
	#endif

	    /* Enter Stop Mode */
	    if (!g_bIsMotion)
	    {
	    	Stop_Callback();
	    }

	    /* If motion is detected, reconfig the system */
	    Wakeup_CallBack();

		/* Print log to indicate that we initialize the program */
		printf("************GTRACK STM32 PROGRAM*************\r\n");

	    /* USER CODE END 2 */
	    /* Infinite loop */
	    /* USER CODE BEGIN WHILE */
	    while (1)
	    {
	    	/* USER CODE END WHILE */

	    	/* USER CODE BEGIN 3 */
		#if (TEST_GPS == 1)

			/* Check whether the inmotion state lasts 5 minutes or not */
			if ((HAL_GetTick() - g_u32CurrentTime) > TIME_5MINUTE)
			{
				/* If the motion is not detected over 5 minutes, the system will enter stop mode */
				Stop_Callback();

				/* Wake up the system if the motion is detected */
				Wakeup_CallBack();
			}
			Data_Process();
		#endif
	  }
}
