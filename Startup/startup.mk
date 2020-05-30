INCLUDES+=-I$(CODE_PATH)/Startup/inc \

OBJECTS+=Startup/startup_stm32f072rbtx.o \


Startup/%.o: $(CODE_PATH)/Startup/%.s
	@echo "Build file $@"
	@mkdir -p ./Startup
	@( $(CC) $(INCLUDES) $(ASMFLAGS) )
