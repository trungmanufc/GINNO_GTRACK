################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Drivers/SC7A20/subdir.mk
-include Drivers/QuectelL76_GPS/subdir.mk
-include Drivers/LTE/subdir.mk
-include Drivers/Flash/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Application/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
G_TRACK_I2C_UART.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
G_TRACK_I2C_UART.list \

OBJCOPY_BIN += \
G_TRACK_I2C_UART.bin \


# All Target
all: G_TRACK_I2C_UART.elf secondary-outputs

# Tool invocations
G_TRACK_I2C_UART.elf: $(OBJS) $(USER_OBJS) C:\Users\Admin\Desktop\GINNO_GTRACK-Tuan_Trung\STM32F401RCTX_FLASH.ld
	arm-none-eabi-gcc -o "G_TRACK_I2C_UART.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Admin\Desktop\GINNO_GTRACK-Tuan_Trung\STM32F401RCTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="G_TRACK_I2C_UART.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

G_TRACK_I2C_UART.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "G_TRACK_I2C_UART.list"
	@echo 'Finished building: $@'
	@echo ' '

G_TRACK_I2C_UART.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "G_TRACK_I2C_UART.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
