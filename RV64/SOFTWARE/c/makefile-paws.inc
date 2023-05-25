all: $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).P64

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).P64 : $(PROJECT).elf
	$(ARCH)-elf-objcopy -O binary $< $@

$(PROJECT).elf : $(LIBPAWS) $(PRJOBJ) $(PRJSRC)
	$(CC) $(CFLAGS) $(INCLUDE) $(LFLAGS) $(LCONFIG) -o $@ $(PRJOBJ) $(LIBPAWS) $(LIBRARY)

tidy :
	rm -f $(PRJOBJ) *.elf $(PAWSBASE)library/$(BUILD_DIR)/*.o $(PAWSBASE)library/build/crt0.o

new :
	rm -f $(PRJOBJ) *.elf

clean :
	rm -f $(PRJOBJ) *.elf $(PAWSBASE)library/$(BUILD_DIR)/*.o $(PAWSBASE)library/build/crt0.o $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).P64
