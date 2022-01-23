include ../makefile.inc

# specific options
CFLAGS:=$(CFLAGS) -Wno-strict-prototypes -Wno-missing-prototypes -Wno-old-style-definition -Wno-multichar -Wno-overflow -Wno-unused-result

output/MY_BASIC.PAW : build/my_basic.elf
	$(ARCH)-elf-objcopy -O binary $< $@

build/my_basic.elf : build/crt0.o $(LIBPAWS) build/main.o build/my_basic.o
	$(LD) $(LFLAGS) $(LCONFIG) -o $@ build/crt0.o build/main.o build/my_basic.o $(LIBPAWS) $(LIBRARY)

build/main.o : shell/main.c core/my_basic.h build/my_basic.o
	$(CC) $(CFLAGS) -c shell/main.c -o $@

build/my_basic.o : core/my_basic.c core/my_basic.h
	$(CC) $(CFLAGS) -c core/my_basic.c -o $@

$(LIBPAWS) : $(LIBPAWSSOURCE)
	$(CC) $(CFLAGS) -c ../library/PAWSlibrary.c -o $@

build/crt0.o : ../crt0.c
	$(CC) $(CFLAGS) -c ../crt0.c -o $@

clean :
	rm -f build/*.o output/*.PAW
