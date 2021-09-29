################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/SC7A20/SC7A20.c 

OBJS += \
./Drivers/SC7A20/SC7A20.o 

C_DEPS += \
./Drivers/SC7A20/SC7A20.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/SC7A20/SC7A20.o: ../Drivers/SC7A20/SC7A20.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Application" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/QuectelL76_GPS" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/SC7A20" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Middlewares" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/LTE" -I"C:/Users/Admin/Desktop/GINNO_GTRACK-Tuan_Trung/Drivers/Flash" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/SC7A20/SC7A20.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

