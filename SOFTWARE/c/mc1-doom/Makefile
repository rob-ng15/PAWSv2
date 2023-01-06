include ../makefile.inc

PROJECT=DOOM
PROJECTTYPE=GAMES

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
$(BUILD_DIR)/am_map.o \
$(BUILD_DIR)/d_items.o \
$(BUILD_DIR)/d_main.o \
$(BUILD_DIR)/doomdef.o \
$(BUILD_DIR)/doomstat.o \
$(BUILD_DIR)/dstrings.o \
$(BUILD_DIR)/f_finale.o \
$(BUILD_DIR)/f_wipe.o \
$(BUILD_DIR)/g_game.o \
$(BUILD_DIR)/hu_lib.o \
$(BUILD_DIR)/hu_stuff.o \
$(BUILD_DIR)/i_main.o \
$(BUILD_DIR)/i_net_dummy.o \
$(BUILD_DIR)/info.o \
$(BUILD_DIR)/i_sound_dummy.o \
$(BUILD_DIR)/i_system.o \
$(BUILD_DIR)/i_video_fb.o \
$(BUILD_DIR)/m_argv.o \
$(BUILD_DIR)/m_bbox.o \
$(BUILD_DIR)/m_cheat.o \
$(BUILD_DIR)/m_fixed.o \
$(BUILD_DIR)/m_menu.o \
$(BUILD_DIR)/m_misc.o \
$(BUILD_DIR)/m_random.o \
$(BUILD_DIR)/m_swap.o \
$(BUILD_DIR)/p_ceilng.o \
$(BUILD_DIR)/p_doors.o \
$(BUILD_DIR)/p_enemy.o \
$(BUILD_DIR)/p_floor.o \
$(BUILD_DIR)/p_inter.o \
$(BUILD_DIR)/p_lights.o \
$(BUILD_DIR)/p_map.o \
$(BUILD_DIR)/p_maputl.o \
$(BUILD_DIR)/p_mobj.o \
$(BUILD_DIR)/p_plats.o \
$(BUILD_DIR)/p_pspr.o \
$(BUILD_DIR)/p_saveg.o \
$(BUILD_DIR)/p_setup.o \
$(BUILD_DIR)/p_sight.o \
$(BUILD_DIR)/p_spec.o \
$(BUILD_DIR)/p_switch.o \
$(BUILD_DIR)/p_telept.o \
$(BUILD_DIR)/p_tick.o \
$(BUILD_DIR)/p_user.o \
$(BUILD_DIR)/r_bsp.o \
$(BUILD_DIR)/r_data.o \
$(BUILD_DIR)/r_draw.o \
$(BUILD_DIR)/r_main.o \
$(BUILD_DIR)/r_plane.o \
$(BUILD_DIR)/r_segs.o \
$(BUILD_DIR)/r_sky.o \
$(BUILD_DIR)/r_things.o \
$(BUILD_DIR)/sounds.o \
$(BUILD_DIR)/s_sound.o \
$(BUILD_DIR)/st_lib.o \
$(BUILD_DIR)/st_stuff.o \
$(BUILD_DIR)/tables.o \
$(BUILD_DIR)/v_video.o \
$(BUILD_DIR)/wi_stuff.o \
$(BUILD_DIR)/w_wad.o \
$(BUILD_DIR)/z_zone.o

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW :$(BUILD_DIR)/doom.elf
	$(ARCH)-elf-objcopy -O binary $< $@
	@echo ""
	@echo "Please copy doom1.wad to the /GAMES directory of the SDCARD"
	@echo "When copying DOOM.PAW"

$(BUILD_DIR)/doom.elf: $(DOOM_OBJECTS) $(LIBPAWS)
	$(CC) $(CFLAGS) $(INCLUDE) $(LFLAGS) $(LCONFIG) -o $@ $(DOOM_BUILD_OBJECTS) $(LIBPAWS) $(LIBRARY)
	chmod -x $@

%.o: src/%.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $(BUILD_DIR)/$@ $<

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

