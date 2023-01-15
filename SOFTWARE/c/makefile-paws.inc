all: $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).P64

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).P64 : $(BUILD_DIR)/code.elf
	$(ARCH)-elf-objcopy -O binary $< $@

$(BUILD_DIR)/code.elf : $(LIBPAWS) $(BUILD_DIR)/code.o
	$(CC) $(CFLAGS) $(INCLUDE) $(LFLAGS) $(LCONFIG) -o $@ $(BUILD_DIR)/code.o $(LIBPAWS) $(LIBRARY)

$(BUILD_DIR)/code.o : $(SOURCE)
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SOURCE) -o $@

clean :
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).P64 \
		  $(PAWSBASE)library/$(BUILD_DIR)/*.o $(PAWSBASE)library/build/crt0.o

