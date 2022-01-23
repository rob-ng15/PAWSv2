ARCH=riscv64
AR=$(ARCH)-elf-ar
CC=$(ARCH)-elf-gcc
LD=$(ARCH)-elf-ld
NM=$(ARCH)--elf-nm
RANLIB=$(ARCH)--elf-ranlib

CFLAGS=-march=rv32imafc -mabi=ilp32f -ffunction-sections -fdata-sections -Ofast -fno-pic -fno-unroll-loops -Wno-stringop-overread
LFLAGS=--as-needed --gc-sections -m elf32lriscv -b elf32-littleriscv --no-relax
LCONFIG=-Tconfig_c_SDRAM.ld
INCLUDE=-I/usr/riscv32-elf/include/
LIBRARY=/usr/riscv64-elf/lib/rv32imafc/ilp32f/libc.a /usr/riscv64-elf/lib/rv32imafc/ilp32f/libm.a /usr/lib/gcc/riscv64-elf/11.1.0/rv32imafc/ilp32f/libgcc.a

# specific options
CFLAGS:=$(CFLAGS) -Wno-strict-prototypes -Wno-missing-prototypes -Wno-old-style-definition

all: doom.elf

DOOM_OBJECTS= \
am_map.o \
d_items.o \
d_main.o \
doomdef.o \
doomstat.o \
dstrings.o \
f_finale.o \
f_wipe.o \
g_game.o \
hu_lib.o \
hu_stuff.o \
i_main.o \
i_net_dummy.o \
info.o \
i_sound_dummy.o \
i_system.o \
i_video_fb.o \
m_argv.o \
m_bbox.o \
m_cheat.o \
m_fixed.o \
m_menu.o \
m_misc.o \
m_random.o \
m_swap.o \
p_ceilng.o \
p_doors.o \
p_enemy.o \
p_floor.o \
p_inter.o \
p_lights.o \
p_map.o \
p_maputl.o \
p_mobj.o \
p_plats.o \
p_pspr.o \
p_saveg.o \
p_setup.o \
p_sight.o \
p_spec.o \
p_switch.o \
p_telept.o \
p_tick.o \
p_user.o \
r_bsp.o \
r_data.o \
r_draw.o \
r_main.o \
r_plane.o \
r_segs.o \
r_sky.o \
r_things.o \
sounds.o \
s_sound.o \
st_lib.o \
st_stuff.o \
tables.o \
v_video.o \
wi_stuff.o \
w_wad.o \
z_zone.o

DOOM_BUILD_OBJECTS= \
build/am_map.o \
build/d_items.o \
build/d_main.o \
build/doomdef.o \
build/doomstat.o \
build/dstrings.o \
build/f_finale.o \
build/f_wipe.o \
build/g_game.o \
build/hu_lib.o \
build/hu_stuff.o \
build/i_main.o \
build/i_net_dummy.o \
build/info.o \
build/i_sound_dummy.o \
build/i_system.o \
build/i_video_fb.o \
build/m_argv.o \
build/m_bbox.o \
build/m_cheat.o \
build/m_fixed.o \
build/m_menu.o \
build/m_misc.o \
build/m_random.o \
build/m_swap.o \
build/p_ceilng.o \
build/p_doors.o \
build/p_enemy.o \
build/p_floor.o \
build/p_inter.o \
build/p_lights.o \
build/p_map.o \
build/p_maputl.o \
build/p_mobj.o \
build/p_plats.o \
build/p_pspr.o \
build/p_saveg.o \
build/p_setup.o \
build/p_sight.o \
build/p_spec.o \
build/p_switch.o \
build/p_telept.o \
build/p_tick.o \
build/p_user.o \
build/r_bsp.o \
build/r_data.o \
build/r_draw.o \
build/r_main.o \
build/r_plane.o \
build/r_segs.o \
build/r_sky.o \
build/r_things.o \
build/sounds.o \
build/s_sound.o \
build/st_lib.o \
build/st_stuff.o \
build/tables.o \
build/v_video.o \
build/wi_stuff.o \
build/w_wad.o \
build/z_zone.o

doom.elf: $(DOOM_OBJECTS) crt0.o libPAWS.o
	$(LD) $(LFLAGS) $(LCONFIG) -o $@ $(DOOM_BUILD_OBJECTS) build/crt0.o build/libPAWS.o $(LIBRARY)
	chmod -x $@

libPAWS.o: ../library/PAWSlibrary.c
	$(CC) $(CFLAGS) -c -o build/$@ $<

crt0.o: ../../crt0.c
	$(CC) $(CFLAGS) -c -o build/$@ $<

%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o build/$@ $<
