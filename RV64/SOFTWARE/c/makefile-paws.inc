all: $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).P64

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).P64 : $(PROJECT).elf
	$(ARCH)-elf-objcopy -O binary $< $@

$(PROJECT).elf : $(LIBPAWS) $(PRJOBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $(LFLAGS) $(LCONFIG) -o $@ $(PRJOBJ) $(LIBPAWS) $(LIBRARY)

tidy :
	rm -f $(PRJOBJ) *.elf $(PAWSBASE)library/$(BUILD_DIR)/*.o $(PAWSBASE)library/build/crt0.o

clean :
	rm -f $(PRJOBJ) *.elf $(PAWSBASE)library/$(BUILD_DIR)/*.o $(PAWSBASE)library/build/crt0.o $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).P64
