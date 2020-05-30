INCLUDES+=-I$(CODE_PATH)/Code/inc \

OBJECTS+=Code/global.o \
         Code/main.o \
         Code/prs_configure.o \
         Code/prs_interrupts.o \
         Code/prs_tasks.o \

Code/%.o: $(CODE_PATH)/Code/src/%.c
	@echo "Build file $@"
	@mkdir -p ./Code
	@( $(CC) $(INCLUDES) $(CFLAGS) )
