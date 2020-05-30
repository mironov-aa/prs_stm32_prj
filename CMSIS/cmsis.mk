INCLUDES+=-I$(CODE_PATH)/CMSIS/inc \

OBJECTS+=CMSIS/system_stm32f0xx.o \

CMSIS/%.o: $(CODE_PATH)/CMSIS/src/%.c
	@echo "Build file $@"
	@mkdir -p ./CMSIS
	@( $(CC) $(INCLUDES) $(CFLAGS) )
