INCLUDES+=-I$(CODE_PATH)/FreeRTOS/inc \

OBJECTS+=FreeRTOS/croutine.o \
         FreeRTOS/event_groups.o \
         FreeRTOS/list.o \
         FreeRTOS/port.o \
         FreeRTOS/queue.o \
         FreeRTOS/stream_buffer.o \
         FreeRTOS/tasks.o \
         FreeRTOS/timers.o \

FreeRTOS/%.o: $(CODE_PATH)/FreeRTOS/src/%.c
	@echo "Build file $@"
	@mkdir -p ./FreeRTOS
	@( $(CC) $(INCLUDES) $(CFLAGS) )
