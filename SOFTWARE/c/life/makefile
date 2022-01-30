include ../makefile.inc

PROJECT=LIFE
PROJECTTYPE=DEMO
SOURCE=life.c

all: $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW : $(BUILD_DIR)/code.elf
	$(ARCH)-elf-objcopy -O binary $< $@

$(BUILD_DIR)/code.elf : $(BUILD_DIR)/crt0.o $(LIBPAWS) $(BUILD_DIR)/code.o
	$(LD) $(LFLAGS) $(LCONFIG) -o $@ $(BUILD_DIR)/crt0.o $(BUILD_DIR)/code.o $(LIBPAWS) $(LIBRARY)

$(BUILD_DIR)/code.o : $(SOURCE)
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SOURCE) -o $@

clean :
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW
