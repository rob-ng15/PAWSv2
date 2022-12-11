include ../makefile.inc

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
	in_paws.o \
	keys.o \
	menu.o \
	mathlib.o \
	model.o \
	net_main.o \
	net_none.o \
	net_loop.o \
	net_vcr.o \
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
	sys_paws.o \
	vid_paws.o \
	snd_paws.o \

SQUAKE_BUILD_OBJECTS = \
	$(BUILD_DIR)/cl_demo.o \
	$(BUILD_DIR)/cl_input.o \
	$(BUILD_DIR)/cl_main.o \
	$(BUILD_DIR)/cl_parse.o \
	$(BUILD_DIR)/cl_tent.o \
	$(BUILD_DIR)/chase.o \
	$(BUILD_DIR)/cmd.o \
	$(BUILD_DIR)/common.o \
	$(BUILD_DIR)/console.o \
	$(BUILD_DIR)/crc.o \
	$(BUILD_DIR)/cvar.o \
	$(BUILD_DIR)/draw.o \
	$(BUILD_DIR)/d_edge.o \
	$(BUILD_DIR)/d_fill.o \
	$(BUILD_DIR)/d_init.o \
	$(BUILD_DIR)/d_modech.o \
	$(BUILD_DIR)/d_part.o \
	$(BUILD_DIR)/d_polyse.o \
	$(BUILD_DIR)/d_scan.o \
	$(BUILD_DIR)/d_sky.o \
	$(BUILD_DIR)/d_sprite.o \
	$(BUILD_DIR)/d_surf.o \
	$(BUILD_DIR)/d_vars.o \
	$(BUILD_DIR)/d_zpoint.o \
	$(BUILD_DIR)/host.o \
	$(BUILD_DIR)/host_cmd.o \
	$(BUILD_DIR)/in_paws.o \
	$(BUILD_DIR)/keys.o \
	$(BUILD_DIR)/menu.o \
	$(BUILD_DIR)/mathlib.o \
	$(BUILD_DIR)/model.o \
	$(BUILD_DIR)/net_main.o \
	$(BUILD_DIR)/net_loop.o \
	$(BUILD_DIR)/net_none.o \
	$(BUILD_DIR)/net_vcr.o \
	$(BUILD_DIR)/nonintel.o \
	$(BUILD_DIR)/pr_cmds.o \
	$(BUILD_DIR)/pr_edict.o \
	$(BUILD_DIR)/pr_exec.o \
	$(BUILD_DIR)/r_aclip.o \
	$(BUILD_DIR)/r_alias.o \
	$(BUILD_DIR)/r_bsp.o \
	$(BUILD_DIR)/r_light.o \
	$(BUILD_DIR)/r_draw.o \
	$(BUILD_DIR)/r_efrag.o \
	$(BUILD_DIR)/r_edge.o \
	$(BUILD_DIR)/r_misc.o \
	$(BUILD_DIR)/r_main.o \
	$(BUILD_DIR)/r_sky.o \
	$(BUILD_DIR)/r_sprite.o \
	$(BUILD_DIR)/r_surf.o \
	$(BUILD_DIR)/r_part.o \
	$(BUILD_DIR)/r_vars.o \
	$(BUILD_DIR)/screen.o \
	$(BUILD_DIR)/sbar.o \
	$(BUILD_DIR)/sv_main.o \
	$(BUILD_DIR)/sv_phys.o \
	$(BUILD_DIR)/sv_move.o \
	$(BUILD_DIR)/sv_user.o \
	$(BUILD_DIR)/zone.o	\
	$(BUILD_DIR)/view.o	\
	$(BUILD_DIR)/wad.o \
	$(BUILD_DIR)/world.o \
	$(BUILD_DIR)/cd_null.o \
	$(BUILD_DIR)/sys_paws.o \
	$(BUILD_DIR)/vid_paws.o \
	$(BUILD_DIR)/snd_paws.o \

$(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW :$(BUILD_DIR)/squake.elf
	$(ARCH)-elf-objcopy -O binary $< $@
	@echo ""
	@echo ""
	@echo ""

$(BUILD_DIR)/squake.elf: $(SQUAKE_OBJECTS) $(LIBPAWS)
	$(CC) $(CFLAGS) $(INCLUDE) $(LFLAGS) $(LCONFIG) -o $@ $(SQUAKE_BUILD_OBJECTS) $(LIBPAWS) $(LIBRARY)
	chmod -x $@

%.o: WinQuake/%.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $(BUILD_DIR)/$@ $<

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(OUTPUT_DIR)$(PROJECTTYPE)/$(PROJECT).PAW

