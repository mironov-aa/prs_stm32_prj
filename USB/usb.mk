INCLUDES+=-I$(CODE_PATH)/USB/Drivers/inc \
          -I$(CODE_PATH)/USB/Library/inc \
          -I$(CODE_PATH)/USB/MCS_Class/inc \
          -I$(CODE_PATH)/USB/User/inc \

OBJECTS+=USB/Drivers/usb_core.o \
         USB/Drivers/usb_dcd.o \
         USB/Drivers/usb_dcd_int.o \
         USB/Library/usbd_core.o \
         USB/Library/usbd_ioreq.o \
         USB/Library/usbd_req.o \
         USB/MCS_Class/usbd_msc_bot.o \
         USB/MCS_Class/usbd_msc_core.o \
         USB/MCS_Class/usbd_msc_data.o \
         USB/MCS_Class/usbd_msc_scsi.o \
         USB/User/usb_bsp.o \
         USB/User/usbd_desc.o \
         USB/User/usbd_pwr.o \
         USB/User/usbd_storage_msd.o \
         USB/User/usbd_usr.o \

USB/Drivers/%.o: $(CODE_PATH)/USB/Drivers/src/%.c
	@echo "Build file $@"
	@mkdir -p ./USB/Drivers
	@( $(CC) $(INCLUDES) $(CFLAGS) )

USB/Library/%.o: $(CODE_PATH)/USB/Library/src/%.c
	@echo "Build file $@"
	@mkdir -p ./USB/Library
	@( $(CC) $(INCLUDES) $(CFLAGS) )
	
USB/MCS_Class/%.o: $(CODE_PATH)/USB/MCS_Class/src/%.c
	@echo "Build file $@"
	@mkdir -p ./USB/MCS_Class
	@( $(CC) $(INCLUDES) $(CFLAGS) )
	
USB/User/%.o: $(CODE_PATH)/USB/User/src/%.c
	@echo "Build file $@"
	@mkdir -p ./USB/User
	@( $(CC) $(INCLUDES) $(CFLAGS) )
	
