################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include FreeRTOS/src/subdir.mk
-include Code/src/subdir.mk
-include CMSIS/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Diploma_optimus.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Diploma_optimus.list \

OBJCOPY_BIN += \
Diploma_optimus.bin \


# All Target
all: Diploma_optimus.elf secondary-outputs

# Tool invocations
Diploma_optimus.elf: $(OBJS) $(USER_OBJS) /home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/STM32F072RBTX_FLASH.ld
	arm-none-eabi-gcc -o "Diploma_optimus.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/STM32F072RBTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Diploma_optimus.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Diploma_optimus.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Diploma_optimus.list"
	@echo 'Finished building: $@'
	@echo ' '

Diploma_optimus.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Diploma_optimus.bin"
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
