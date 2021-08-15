################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../QuectelL76_GPS/L76.c 

OBJS += \
./QuectelL76_GPS/L76.o 

C_DEPS += \
./QuectelL76_GPS/L76.d 


# Each subdirectory must supply rules for building sources it contributes
QuectelL76_GPS/%.o: ../QuectelL76_GPS/%.c QuectelL76_GPS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"D:/20202/NHUNG/Ginno Internship/G_TRACK_I2C_UART/QuectelL76_GPS" -I"D:/20202/NHUNG/Ginno Internship/G_TRACK_I2C_UART/SC7A20" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

