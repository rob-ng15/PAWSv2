all: $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW : $(PROJECT).elf
	$(ARCH)-elf-objcopy -O binary $< $@

$(PROJECT).elf : $(LIBPAWS) $(PRJOBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $(LFLAGS) $(LCONFIG) -o $@ $(PRJOBJ) $(LIBPAWS) $(LIBRARY)

new :
	rm -f $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

clean :
	rm -f $(PRJOBJ) *.elf $(PAWSBASE)library/$(BUILD_DIR)/*.o $(PAWSBASE)library/build/crt0.o
