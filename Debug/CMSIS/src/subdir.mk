################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/src/system_stm32f0xx.c 

OBJS += \
./CMSIS/src/system_stm32f0xx.o 

C_DEPS += \
./CMSIS/src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/src/system_stm32f0xx.o: ../CMSIS/src/system_stm32f0xx.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32 -DSTM32F0 -DSTM32F072RBTx -DSTM32F072B_DISCO -DDEBUG -c -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/CMSIS/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/Code/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/FreeRTOS/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"CMSIS/src/system_stm32f0xx.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

