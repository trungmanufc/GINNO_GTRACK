################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/QuectelL76_GPS/L76.c 

OBJS += \
./Drivers/QuectelL76_GPS/L76.o 

C_DEPS += \
./Drivers/QuectelL76_GPS/L76.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/QuectelL76_GPS/L76.o: ../Drivers/QuectelL76_GPS/L76.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Application" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/QuectelL76_GPS" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/SC7A20" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Middlewares" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/LTE" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/Flash" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/QuectelL76_GPS/L76.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

