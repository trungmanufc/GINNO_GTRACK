################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LTE/lte.c \
../LTE/soft_uart.c \
../LTE/stm32f4xx_hal_uart_emul.c 

OBJS += \
./LTE/lte.o \
./LTE/soft_uart.o \
./LTE/stm32f4xx_hal_uart_emul.o 

C_DEPS += \
./LTE/lte.d \
./LTE/soft_uart.d \
./LTE/stm32f4xx_hal_uart_emul.d 


# Each subdirectory must supply rules for building sources it contributes
LTE/%.o: ../LTE/%.c LTE/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"D:/20202/NHUNG/Ginno Internship/G_TRACK_I2C_UART/LTE" -I"D:/20202/NHUNG/Ginno Internship/G_TRACK_I2C_UART/Flash" -I"D:/20202/NHUNG/Ginno Internship/G_TRACK_I2C_UART/QuectelL76_GPS" -I"D:/20202/NHUNG/Ginno Internship/G_TRACK_I2C_UART/SC7A20" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

