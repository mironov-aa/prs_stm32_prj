################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Code/src/main.c 

OBJS += \
./Code/src/main.o 

C_DEPS += \
./Code/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Code/src/main.o: ../Code/src/main.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32 -DSTM32F0 -DSTM32F072RBTx -DSTM32F072B_DISCO -DDEBUG -c -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/CMSIS/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/Code/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/FreeRTOS/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/src/main.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

