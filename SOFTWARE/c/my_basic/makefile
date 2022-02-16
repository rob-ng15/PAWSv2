include ../makefile.inc

PROJECT=MY_BASIC
PROJECTTYPE=COMPUTER

all: $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

# specific options
CFLAGS:=$(CFLAGS) -Wno-strict-prototypes -Wno-missing-prototypes -Wno-old-style-definition -Wno-multichar -Wno-overflow -Wno-unused-result

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW : $(BUILD_DIR)/my_basic.elf
	$(ARCH)-elf-objcopy -O binary $< $@

$(BUILD_DIR)/my_basic.elf : $(LIBPAWS) $(BUILD_DIR)/main.o $(BUILD_DIR)/my_basic.o
	$(LD) $(LFLAGS) $(LCONFIG) -o $@ $(BUILD_DIR)/main.o $(BUILD_DIR)/my_basic.o $(LIBPAWS) $(LIBRARY)

$(BUILD_DIR)/main.o : shell/main.c core/my_basic.h $(BUILD_DIR)/my_basic.o
	$(CC) $(CFLAGS) $(INCLUDE) -c shell/main.c -o $@

$(BUILD_DIR)/my_basic.o : core/my_basic.c core/my_basic.h
	$(CC) $(CFLAGS) $(INCLUDE) -c core/my_basic.c -o $@

clean :
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW
