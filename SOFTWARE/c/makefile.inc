OS := $(shell uname -s)
IS_APPLE := $(shell echo $(OS)|grep -i darwin)

everything: all

ARCH=riscv64
AR=$(ARCH)-elf-ar
CC=$(ARCH)-elf-gcc
LD=$(ARCH)-elf-ld.bfd
NM=$(ARCH)--elf-nm
RANLIB=$(ARCH)--elf-ranlib

GCCVER=12.1.0

BUILD_DIR=build
OUTPUT_DIR=../../PAWS/

CFLAGS=-march=rv64imafd_zifencei_zicsr -mabi=lp64d -mshorten-memrefs -mrelax -ffunction-sections -fdata-sections -O2 -finline -fno-pic -fno-unroll-loops -Wno-stringop-overread -nostdlib
LFLAGS=-Wl,--as-needed -Wl,--gc-sections -Wl,-melf64lriscv -Wl,-belf64-littleriscv
LCONFIG=-Wl,-T../config_c_SDRAM.ld

PAWSBASE=../
INCLUDE=-I$(PAWSBASE)/library/ -I$(PAWSBASE)/library/newlib/ -I/usr/riscv64-elf/include/

LIBRARY=/usr/riscv64-elf/lib/rv64imafdc/lp64d/libc.a \
		/usr/riscv64-elf/lib/rv64imafdc/lp64d/libm.a \
		/usr/lib/gcc/riscv64-elf/$(GCCVER)/rv64imafdc/lp64d/libgcc.a \

CRT0SOURCE=	$(PAWSBASE)library/crt0.c
CRT0OBJ=	$(PAWSBASE)library/build/crt0.o

LIBPAWSSOURCE=	$(PAWSBASE)library/PAWS.h $(PAWSBASE)library/PAWSdefinitions.h $(PAWSBASE)library/PAWSlibrary.c
LIBPAWSOBJ=	$(PAWSBASE)library/$(BUILD_DIR)/libPAWS.o

PAWSP2SOURCE=	$(PAWSBASE)library/PAWSPS2driver.c
PAWSPS2OBJ=	$(PAWSBASE)library/$(BUILD_DIR)/PAWSPS2driver.o

NANOJPEGSOURCE= $(PAWSBASE)library/nanojpeg/nanojpeg.c
NANOJPEGOBJ=	$(PAWSBASE)library/$(BUILD_DIR)/nanojpeg.o

FATIOLIBSOURCE= $(wildcard $(PAWSBASE)library/fat_io_lib/*.c) $(wildcard $(PAWSBASE)library/fat_io_lib/*.h)
FATIOLIBOBJ=	$(PAWSBASE)library/$(BUILD_DIR)/fat_filelib.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_access.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_table.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_write.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_misc.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_string.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_cache.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_format.o

LIBPAWS= $(LIBPAWSOBJ) $(PAWSPS2OBJ) $(NANOJPEGOBJ) $(FATIOLIBOBJ)

$(LIBPAWSOBJ) : $(LIBPAWSSOURCE) $(CRT0OBJ)
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/PAWSlibrary.c -o $@

$(PAWSPS2OBJ) : $(PAWSP2SOURCE)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(NANOJPEGOBJ) : $(NANOJPEGSOURCE)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(FATIOLIBOBJ) : $(FATIOLIBSOURCE)
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_filelib.c -o $(PAWSBASE)library/$(BUILD_DIR)/fat_filelib.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_access.c -o $(PAWSBASE)library/$(BUILD_DIR)/fat_access.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_table.c -o $(PAWSBASE)library/$(BUILD_DIR)/fat_table.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_write.c -o $(PAWSBASE)library/$(BUILD_DIR)/fat_write.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_misc.c -o $(PAWSBASE)library/$(BUILD_DIR)/fat_misc.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_string.c -o $(PAWSBASE)library/$(BUILD_DIR)/fat_string.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_cache.c -o $(PAWSBASE)library/$(BUILD_DIR)/fat_cache.o
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/fat_io_lib/fat_format.c -o $(PAWSBASE)library/$(BUILD_DIR)/fat_format.o

$(CRT0OBJ) : $(CRT0SOURCE)
	$(CC) $(CFLAGS) -c $< -o $@
