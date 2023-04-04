OS := $(shell uname -s)
IS_APPLE := $(shell echo $(OS)|grep -i darwin)

everything: all

ARCH=riscv64
AR=$(ARCH)-elf-ar
CC=$(ARCH)-elf-gcc
CXX=$(ARCH)-elf-g++
LD=$(ARCH)-elf-ld.bfd
NM=$(ARCH)--elf-nm
RANLIB=$(ARCH)--elf-ranlib

GCCVER=12.2.0
COMPILER=$(CC)

BUILD_DIR=build
OUTPUT_DIR=../../PAWS/

ifndef CFLAGSEXTRA
CFLAGSEXTRA=
endif
ifndef INCLUDEEXTRA
INCLUDEEXTRA=
endif

CPUOPTS=-march=rv64gc_zba_zbb_zbc_zbs_zbkb_zbkc_zifencei_zicsr -mabi=lp64d -mshorten-memrefs -mrelax

CFLAGS=$(CPUOPTS) -O2 -ffunction-sections -fdata-sections -finline -fno-pic -fno-unroll-loops -Wno-stringop-overread -nostdlib $(CFLAGSEXTRA)
CXXFLAGS=$(CPUOPTS) -O2 -ffunction-sections -fdata-sections -finline -fno-pic -fno-unroll-loops -fno-threadsafe-statics -fno-rtti -fno-exceptions -Wno-stringop-overread -nostdlib
LFLAGS=-Wl,--as-needed -Wl,--gc-sections -Wl,-melf64lriscv -Wl,-belf64-littleriscv \
	-Wl,--wrap=memcpy -Wl,--wrap=memset \
	-Wl,--wrap=__clzsi2 -Wl,--wrap=__clzdi2 \
	-Wl,--wrap=__ctzsi2 -Wl,--wrap=__ctzdi2 \
	-Wl,--wrap=__popcountsi2 -Wl,--wrap=__popcountdi2 \
	-Wl,--wrap=__builtin_clz -Wl,--wrap=__builtin_clzl \
	-Wl,--wrap=__builtin_ctz -Wl,--wrap=__builtin_ctzl \
	-Wl,--wrap=__builtin_popcount -Wl,--wrap=__builtin_popcountl \
	-Wl,--wrap=__builtin_parity -Wl,--wrap=__builtin_parityl \
	-Wl,--wrap=__builtin_bswap16 -Wl,--wrap=__builtin_bswap32 -Wl,--wrap=__builtin_bswap64
LCONFIG=-Wl,-T../config_c_SDRAM.ld

PAWSBASE=../
INCLUDE=-I$(PAWSBASE)/library/ -I$(PAWSBASE)/library/newlib/ -I$(PAWSBASE)/library/imgui/ -I$(INCLUDEEXTRA) -I/usr/riscv64-elf/include/

LIBRARY=	/usr/riscv64-elf/lib/rv64imafdc/lp64d/libc.a \
		/usr/riscv64-elf/lib/rv64imafdc/lp64d/libm.a \
		/usr/lib/gcc/riscv64-elf/$(GCCVER)/rv64imafdc/lp64d/libgcc.a \
		/usr/lib/gcc/riscv64-elf/$(GCCVER)/rv64imafdc/lp64d/crtbegin.o \
		/usr/lib/gcc/riscv64-elf/$(GCCVER)/rv64imafdc/lp64d/crtend.o

CRT0SOURCE=	$(PAWSBASE)library/crt0.s
CRT0OBJ=	$(PAWSBASE)library/build/crt0.o

LIBPAWSSOURCE=	$(PAWSBASE)library/PAWS.h $(PAWSBASE)library/PAWSdefinitions.h $(PAWSBASE)library/PAWSlibrary.c
LIBPAWSOBJ=	$(PAWSBASE)library/$(BUILD_DIR)/libPAWS.o

PAWSPS2SOURCE=	$(PAWSBASE)library/PAWSPS2driver.c
PAWSPS2OBJ=	$(PAWSBASE)library/$(BUILD_DIR)/PAWSPS2driver.o

NANOJPEGSOURCE= $(PAWSBASE)library/nanojpeg/nanojpeg.c
NANOJPEGOBJ=	$(PAWSBASE)library/$(BUILD_DIR)/nanojpeg.o

IMGUISOURCE=	$(wildcard $(PAWSBASE)library/imgui/*.cpp) $(wildcard $(PAWSBASE)library/imgui/*.h)
IMGUIOBJ=	$(PAWSBASE)library/$(BUILD_DIR)/imgui_draw.o \
		$(PAWSBASE)library/$(BUILD_DIR)/imgui_sw.o \
		$(PAWSBASE)library/$(BUILD_DIR)/imgui_tables.o \
		$(PAWSBASE)library/$(BUILD_DIR)/imgui_widgets.o \
		$(PAWSBASE)library/$(BUILD_DIR)/imgui.o \
		$(PAWSBASE)library/$(BUILD_DIR)/imgui_demo.o

FATIOLIBSOURCE= $(wildcard $(PAWSBASE)library/fat_io_lib/*.c) $(wildcard $(PAWSBASE)library/fat_io_lib/*.h)
FATIOLIBOBJ=	$(PAWSBASE)library/$(BUILD_DIR)/fat_filelib.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_access.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_table.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_write.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_misc.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_string.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_cache.o \
		$(PAWSBASE)library/$(BUILD_DIR)/fat_format.o

LIBPAWS= $(LIBPAWSOBJ) $(PAWSPS2OBJ) $(NANOJPEGOBJ) $(FATIOLIBOBJ) $(IMGUIOBJ)

PRJSRC=

$(LIBPAWSOBJ) : $(LIBPAWSSOURCE) $(CRT0OBJ)
	$(CC) $(CFLAGS) -c $(PAWSBASE)library/PAWSlibrary.c -o $@

$(PAWSPS2OBJ) : $(PAWSPS2SOURCE)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(NANOJPEGOBJ) : $(NANOJPEGSOURCE)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(IMGUIOBJ) : $(IMGUISOURCE)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $(PAWSBASE)library/imgui/imgui_draw.cpp -o $(PAWSBASE)library/$(BUILD_DIR)/imgui_draw.o
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $(PAWSBASE)library/imgui/imgui_sw.cpp -o $(PAWSBASE)library/$(BUILD_DIR)/imgui_sw.o
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $(PAWSBASE)library/imgui/imgui_tables.cpp -o $(PAWSBASE)library/$(BUILD_DIR)/imgui_tables.o
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $(PAWSBASE)library/imgui/imgui_widgets.cpp -o $(PAWSBASE)library/$(BUILD_DIR)/imgui_widgets.o
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $(PAWSBASE)library/imgui/imgui.cpp -o $(PAWSBASE)library/$(BUILD_DIR)/imgui.o
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $(PAWSBASE)library/imgui/imgui_demo.cpp -o $(PAWSBASE)library/$(BUILD_DIR)/imgui_demo.o

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

$(PRJOBJ)%.o : $(PRJSRC)%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(PRJOBJ)%.o : $(PRJSRC)%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

