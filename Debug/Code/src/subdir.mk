################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Code/src/PRMS_Initialize.c \
../Code/src/PRMS_Interrupts.c \
../Code/src/PRMS_Tasks.c \
../Code/src/main.c 

OBJS += \
./Code/src/PRMS_Initialize.o \
./Code/src/PRMS_Interrupts.o \
./Code/src/PRMS_Tasks.o \
./Code/src/main.o 

C_DEPS += \
./Code/src/PRMS_Initialize.d \
./Code/src/PRMS_Interrupts.d \
./Code/src/PRMS_Tasks.d \
./Code/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Code/src/PRMS_Initialize.o: ../Code/src/PRMS_Initialize.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32 -DSTM32F0 -DSTM32F072RBTx -DSTM32F072B_DISCO -DDEBUG -c -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/CMSIS/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/Code/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/FreeRTOS/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"Code/src/PRMS_Initialize.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/PRMS_Interrupts.o: ../Code/src/PRMS_Interrupts.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32 -DSTM32F0 -DSTM32F072RBTx -DSTM32F072B_DISCO -DDEBUG -c -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/CMSIS/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/Code/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/FreeRTOS/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"Code/src/PRMS_Interrupts.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/PRMS_Tasks.o: ../Code/src/PRMS_Tasks.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32 -DSTM32F0 -DSTM32F072RBTx -DSTM32F072B_DISCO -DDEBUG -c -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/CMSIS/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/Code/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/FreeRTOS/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"Code/src/PRMS_Tasks.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Code/src/main.o: ../Code/src/main.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32 -DSTM32F0 -DSTM32F072RBTx -DSTM32F072B_DISCO -DDEBUG -c -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/CMSIS/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/Code/inc" -I"/home/mironov-aa/Development/STM32CubeIDE/Diploma_optimus/FreeRTOS/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"Code/src/main.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

