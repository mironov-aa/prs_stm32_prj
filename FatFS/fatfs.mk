INCLUDES+=-I$(CODE_PATH)/FatFS/inc \

OBJECTS+=FatFS/diskio.o \
         FatFS/ff.o \
         FatFS/ffsystem.o \

FatFS/%.o: $(CODE_PATH)/FatFS/src/%.c
	@echo "Build file $@"
	@mkdir -p ./FatFS
	@( $(CC) $(INCLUDES) $(CFLAGS) )
