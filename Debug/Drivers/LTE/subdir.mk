################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/LTE/lte.c \
../Drivers/LTE/soft_uart.c \
../Drivers/LTE/stm32f4xx_hal_uart_emul.c 

OBJS += \
./Drivers/LTE/lte.o \
./Drivers/LTE/soft_uart.o \
./Drivers/LTE/stm32f4xx_hal_uart_emul.o 

C_DEPS += \
./Drivers/LTE/lte.d \
./Drivers/LTE/soft_uart.d \
./Drivers/LTE/stm32f4xx_hal_uart_emul.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/LTE/lte.o: ../Drivers/LTE/lte.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Application" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/QuectelL76_GPS" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/SC7A20" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Middlewares" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/LTE" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/Flash" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/LTE/lte.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/LTE/soft_uart.o: ../Drivers/LTE/soft_uart.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Application" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/QuectelL76_GPS" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/SC7A20" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Middlewares" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/LTE" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/Flash" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/LTE/soft_uart.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/LTE/stm32f4xx_hal_uart_emul.o: ../Drivers/LTE/stm32f4xx_hal_uart_emul.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Application" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/QuectelL76_GPS" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/SC7A20" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Middlewares" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/LTE" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/Flash" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/LTE/stm32f4xx_hal_uart_emul.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

