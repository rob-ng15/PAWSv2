include ../makefile.inc

PROJECT=D-STONE
PROJECTTYPE=TEST

all: $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

# specific options
CFLAGS:=$(CFLAGS) -Wno-strict-prototypes -Wno-missing-prototypes -Wno-old-style-definition -Wno-multichar -Wno-overflow -Wno-unused-result

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW : $(BUILD_DIR)/dhrystone.elf
	$(ARCH)-elf-objcopy -O binary $< $@

$(BUILD_DIR)/dhrystone.elf : $(LIBPAWS) $(BUILD_DIR)/dhry_1.o $(BUILD_DIR)/dhry_2.o
	$(CC)  $(CFLAGS) $(LFLAGS) $(LCONFIG) -o $@ $(BUILD_DIR)/dhry_1.o $(BUILD_DIR)/dhry_2.o $(LIBPAWS) $(LIBRARY)

$(BUILD_DIR)/dhry_1.o : dhry_1.c
	$(CC) $(CFLAGS) $(INCLUDE) -c dhry_1.c -o $@

$(BUILD_DIR)/dhry_2.o : dhry_2.c
	$(CC) $(CFLAGS) $(INCLUDE) -c dhry_2.c -o $@

clean :
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW
