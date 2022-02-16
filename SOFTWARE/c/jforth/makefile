include ../makefile.inc

PROJECT=JFORTH
PROJECTTYPE=COMPUTER/JFORTH
SOURCE=main.c

all: $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW : $(BUILD_DIR)/main.elf
	$(ARCH)-elf-objcopy -O binary $< $@
	@echo ""
	@echo "Copy the files editor.fth forth.fth unit.fth to the SDCARD"
	@echo "in folder /COMPUTER/JFORTH/"
	@echo ""
	@echo "JFORTH.PAW loads files from /COMPUTER/JFORTH/"
	@echo ""

$(BUILD_DIR)/main.elf : $(LIBPAWS) $(BUILD_DIR)/main.o
	$(LD) $(LFLAGS) $(LCONFIG) -o $@ $(BUILD_DIR)/main.o $(BUILD_DIR)/libforth.o $(BUILD_DIR)/unit.o $(LIBPAWS) $(LIBRARY)

$(BUILD_DIR)/main.o : $(SOURCE) $(BUILD_DIR)/libforth.o $(BUILD_DIR)/unit.o
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SOURCE) -o $@

$(BUILD_DIR)/libforth.o : libforth.c libforth.h
	$(CC) $(CFLAGS) $(INCLUDE) -c libforth.c -o $@

$(BUILD_DIR)/unit.o : unit.c unit.h
	$(CC) $(CFLAGS) $(INCLUDE) -c unit.c -o $@

clean :
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW
