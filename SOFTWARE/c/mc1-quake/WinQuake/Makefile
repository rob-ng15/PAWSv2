OS := $(shell uname -s)
IS_APPLE := $(shell echo $(OS)|grep -i darwin)

everything: all

ARCH=riscv64
AR=$(ARCH)-elf-ar
CC=$(ARCH)-elf-gcc
LD=$(ARCH)-elf-ld.bfd
NM=$(ARCH)--elf-nm
RANLIB=$(ARCH)--elf-ranlib

BUILD_DIR=build
OUTPUT_DIR=../../PAWS/

CFLAGS=-march=rv32imafc_zba_zbb_zbc_zbs_zbkb -mabi=ilp32f -mshorten-memrefs -mrelax -ffunction-sections -fdata-sections -O2 -finline -fno-pic -fno-unroll-loops -Wno-stringop-overread -nostdlib
LFLAGS=-Wl,--as-needed -Wl,--gc-sections -Wl,-melf32lriscv -Wl,-belf32-littleriscv
LCONFIG=-Wl,-T../../config_c_SDRAM.ld

PAWSBASE=../../
INCLUDE=-I$(PAWSBASE)/library/ -I$(PAWSBASE)/library/newlib/ -I/usr/riscv32-elf/include/

LIBRARY=/usr/riscv64-elf/lib/rv32imafc/ilp32f/libc.a \
	/usr/riscv64-elf/lib/rv32imafc/ilp32f/libm.a \
	/usr/lib/gcc/riscv64-elf/12.1.0/rv32imafc/ilp32f/libgcc.a \

CRT0SOURCE=	$(PAWSBASE)library/crt0.c
CRT0OBJ=	$(PAWSBASE)library/$(BUILD_DIR)/crt0.o

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

LIBPAWS=$(CRT0OBJ) $(LIBPAWSOBJ) $(PAWSPS2OBJ) $(NANOJPEGOBJ) $(FATIOLIBOBJ)

$(LIBPAWSOBJ) : $(LIBPAWSSOURCE)
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

PROJECT=QUAKE
PROJECTTYPE=DEMO

all: $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

SQUAKE_OBJECTS= \
	cl_demo.o \
	cl_input.o \
	cl_main.o \
	cl_parse.o \
	cl_tent.o \
	chase.o \
	cmd.o \
	common.o \
	console.o \
	crc.o \
	cvar.o \
	draw.o \
	d_edge.o \
	d_fill.o \
	d_init.o \
	d_modech.o \
	d_part.o \
	d_polyse.o \
	d_scan.o \
	d_sky.o \
	d_sprite.o \
	d_surf.o \
	d_vars.o \
	d_zpoint.o \
	host.o \
	host_cmd.o \
	keys.o \
	menu.o \
	mathlib.o \
	model.o \
	net_main.o \
	net_none.o \
	nonintel.o \
	pr_cmds.o \
	pr_edict.o \
	pr_exec.o \
	r_aclip.o \
	r_alias.o \
	r_bsp.o \
	r_light.o \
	r_draw.o \
	r_efrag.o \
	r_edge.o \
	r_misc.o \
	r_main.o \
	r_sky.o \
	r_sprite.o \
	r_surf.o \
	r_part.o \
	r_vars.o \
	screen.o \
	sbar.o \
	sv_main.o \
	sv_phys.o \
	sv_move.o \
	sv_user.o \
	zone.o	\
	view.o	\
	wad.o \
	world.o \
	cd_null.o \
	sys_mc1.o \
	vid_mc1.o \
	snd_null.o \

SQUAKE_BUILD_OBJS = \
	$(BUILDDIR)/squake/cl_demo.o \
	$(BUILDDIR)/squake/cl_input.o \
	$(BUILDDIR)/squake/cl_main.o \
	$(BUILDDIR)/squake/cl_parse.o \
	$(BUILDDIR)/squake/cl_tent.o \
	$(BUILDDIR)/squake/chase.o \
	$(BUILDDIR)/squake/cmd.o \
	$(BUILDDIR)/squake/common.o \
	$(BUILDDIR)/squake/console.o \
	$(BUILDDIR)/squake/crc.o \
	$(BUILDDIR)/squake/cvar.o \
	$(BUILDDIR)/squake/draw.o \
	$(BUILDDIR)/squake/d_edge.o \
	$(BUILDDIR)/squake/d_fill.o \
	$(BUILDDIR)/squake/d_init.o \
	$(BUILDDIR)/squake/d_modech.o \
	$(BUILDDIR)/squake/d_part.o \
	$(BUILDDIR)/squake/d_polyse.o \
	$(BUILDDIR)/squake/d_scan.o \
	$(BUILDDIR)/squake/d_sky.o \
	$(BUILDDIR)/squake/d_sprite.o \
	$(BUILDDIR)/squake/d_surf.o \
	$(BUILDDIR)/squake/d_vars.o \
	$(BUILDDIR)/squake/d_zpoint.o \
	$(BUILDDIR)/squake/host.o \
	$(BUILDDIR)/squake/host_cmd.o \
	$(BUILDDIR)/squake/keys.o \
	$(BUILDDIR)/squake/menu.o \
	$(BUILDDIR)/squake/mathlib.o \
	$(BUILDDIR)/squake/model.o \
	$(BUILDDIR)/squake/net_main.o \
	$(BUILDDIR)/squake/net_none.o \
	$(BUILDDIR)/squake/nonintel.o \
	$(BUILDDIR)/squake/pr_cmds.o \
	$(BUILDDIR)/squake/pr_edict.o \
	$(BUILDDIR)/squake/pr_exec.o \
	$(BUILDDIR)/squake/r_aclip.o \
	$(BUILDDIR)/squake/r_alias.o \
	$(BUILDDIR)/squake/r_bsp.o \
	$(BUILDDIR)/squake/r_light.o \
	$(BUILDDIR)/squake/r_draw.o \
	$(BUILDDIR)/squake/r_efrag.o \
	$(BUILDDIR)/squake/r_edge.o \
	$(BUILDDIR)/squake/r_misc.o \
	$(BUILDDIR)/squake/r_main.o \
	$(BUILDDIR)/squake/r_sky.o \
	$(BUILDDIR)/squake/r_sprite.o \
	$(BUILDDIR)/squake/r_surf.o \
	$(BUILDDIR)/squake/r_part.o \
	$(BUILDDIR)/squake/r_vars.o \
	$(BUILDDIR)/squake/screen.o \
	$(BUILDDIR)/squake/sbar.o \
	$(BUILDDIR)/squake/sv_main.o \
	$(BUILDDIR)/squake/sv_phys.o \
	$(BUILDDIR)/squake/sv_move.o \
	$(BUILDDIR)/squake/sv_user.o \
	$(BUILDDIR)/squake/zone.o	\
	$(BUILDDIR)/squake/view.o	\
	$(BUILDDIR)/squake/wad.o \
	$(BUILDDIR)/squake/world.o \
	$(BUILDDIR)/squake/cd_null.o \
	$(BUILDDIR)/squake/sys_mc1.o \
	$(BUILDDIR)/squake/vid_mc1.o \
	$(BUILDDIR)/squake/snd_null.o \

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW :$(BUILD_DIR)/squake.elf
	$(ARCH)-elf-objcopy -O binary $< $@
	@echo ""
	@echo ""
	@echo ""

$(BUILD_DIR)/squake.elf: $(SQUAKE_OBJS) $(LIBPAWS)
	$(CC) $(CFLAGS) $(INCLUDE) $(LFLAGS) $(LCONFIG) -o $@ $(SQUAKE_BUILD_OBJS) $(LIBPAWS) $(LIBRARY)
	chmod -x $@

%.o: src/%.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $(BUILD_DIR)/$@ $<
	$(CC) $(INCLUDE) $(CFLAGS) -S -c -o $(BUILD_DIR)/$@.s $<

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

