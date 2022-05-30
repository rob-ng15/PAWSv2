include ../makefile.inc

PROJECT=DOOM
PROJECTTYPE=DEMO

all: $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

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
build-fast/am_map.o \
build-fast/d_items.o \
build-fast/d_main.o \
build-fast/doomdef.o \
build-fast/doomstat.o \
build-fast/dstrings.o \
build-fast/f_finale.o \
build-fast/f_wipe.o \
build-fast/g_game.o \
build-fast/hu_lib.o \
build-fast/hu_stuff.o \
build-fast/i_main.o \
build-fast/i_net_dummy.o \
build-fast/info.o \
build-fast/i_sound_dummy.o \
build-fast/i_system.o \
build-fast/i_video_fb.o \
build-fast/m_argv.o \
build-fast/m_bbox.o \
build-fast/m_cheat.o \
build-fast/m_fixed.o \
build-fast/m_menu.o \
build-fast/m_misc.o \
build-fast/m_random.o \
build-fast/m_swap.o \
build-fast/p_ceilng.o \
build-fast/p_doors.o \
build-fast/p_enemy.o \
build-fast/p_floor.o \
build-fast/p_inter.o \
build-fast/p_lights.o \
build-fast/p_map.o \
build-fast/p_maputl.o \
build-fast/p_mobj.o \
build-fast/p_plats.o \
build-fast/p_pspr.o \
build-fast/p_saveg.o \
build-fast/p_setup.o \
build-fast/p_sight.o \
build-fast/p_spec.o \
build-fast/p_switch.o \
build-fast/p_telept.o \
build-fast/p_tick.o \
build-fast/p_user.o \
build-fast/r_bsp.o \
build-fast/r_data.o \
build-fast/r_draw.o \
build-fast/r_main.o \
build-fast/r_plane.o \
build-fast/r_segs.o \
build-fast/r_sky.o \
build-fast/r_things.o \
build-fast/sounds.o \
build-fast/s_sound.o \
build-fast/st_lib.o \
build-fast/st_stuff.o \
build-fast/tables.o \
build-fast/v_video.o \
build-fast/wi_stuff.o \
build-fast/w_wad.o \
build-fast/z_zone.o

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW :$(BUILD_DIR)/doom.elf
	$(ARCH)-elf-objcopy -O binary $< $@
	@echo ""
	@echo "Please copy doom1.wad to the /DEMO directory of the SDCARD"
	@echo "When copying DOOM.PAW"

$(BUILD_DIR)/doom.elf: $(DOOM_OBJECTS) $(LIBPAWS)
	$(LD) $(LFLAGS) $(LCONFIG) -o $@ $(DOOM_BUILD_OBJECTS) $(LIBPAWS) $(LIBRARY)
	chmod -x $@

%.o: src/%.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $(BUILD_DIR)/$@ $<
	$(CC) $(INCLUDE) $(CFLAGS) -S -c -o $(BUILD_DIR)/$@.s $<

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

