INCLUDES+=-I$(CODE_PATH)/Drivers/inc \

OBJECTS+=Drivers/sdhc_card_driver.o \
         Drivers/spi_driver.o \

Drivers/%.o: $(CODE_PATH)/Drivers/src/%.c
	@echo "Build file $@"
	@mkdir -p ./Drivers
	@( $(CC) $(INCLUDES) $(CFLAGS)  )
