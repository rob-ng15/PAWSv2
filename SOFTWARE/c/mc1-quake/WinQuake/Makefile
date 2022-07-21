#
# Quake Makefile for Linux 2.0
#
# Aug '98 by Zoid <zoid@idsoftware.com>
#
# ELF only
#

BASEVERSION=1.09
VERSION=$(BASEVERSION)

ARCH=riscv64
AR=$(ARCH)-elf-ar
CC=$(ARCH)-elf-gcc
LD=$(ARCH)-elf-ld.bfd
NM=$(ARCH)--elf-nm
RANLIB=$(ARCH)--elf-ranlib
NOARCH=noarch

MOUNT_DIR=./

BUILD_DEBUG_DIR=debug$(ARCH)
BUILD_RELEASE_DIR=release$(ARCH)

CFLAGS=-march=rv32imafc_zba_zbb_zbc_zbs_zbkb -mabi=ilp32f -mshorten-memrefs -mrelax -ffunction-sections -fdata-sections -O2 -finline -fno-pic -fno-unroll-loops -Wno-stringop-overread -nostdlib
LFLAGS=-Wl,--as-needed -Wl,--gc-sections -Wl,-melf32lriscv -Wl,-belf32-littleriscv
LCONFIG=-Wl,-T../config_c_SDRAM.ld

RELEASE_CFLAGS=$(BASE_CFLAGS) -g -fexpensive-optimizations
DEBUG_CFLAGS=$(BASE_CFLAGS) -g

DO_CC=$(CC) $(CFLAGS) -o $@ -c $<
DO_DEBUG_CC=$(CC) $(DEBUG_CFLAGS) -o $@ -c $<
DO_O_CC=$(CC) -O $(CFLAGS) -o $@ -c $<
DO_AS=$(CC) $(CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

TARGETS=$(BUILDDIR)/bin/squake

build_debug:
	@-mkdir $(BUILD_DEBUG_DIR)/squake
	$(MAKE) targets BUILDDIR=$(BUILD_DEBUG_DIR) CFLAGS="$(DEBUG_CFLAGS)"

build_release:
	@-mkdir $(BUILD_RELEASE_DIR)/squake
	$(MAKE) targets BUILDDIR=$(BUILD_RELEASE_DIR) CFLAGS="$(RELEASE_CFLAGS)"

all: build_debug build_release

targets: $(TARGETS)

#############################################################################
# SVGALIB Quake
#############################################################################

SQUAKE_OBJS = \
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
	$(BUILDDIR)/squake/net_dgrm.o \
	$(BUILDDIR)/squake/net_loop.o \
	$(BUILDDIR)/squake/net_main.o \
	$(BUILDDIR)/squake/net_vcr.o \
	$(BUILDDIR)/squake/net_udp.o \
	$(BUILDDIR)/squake/net_bsd.o \
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
	$(BUILDDIR)/squake/cd_linux.o \
	$(BUILDDIR)/squake/sys_linux.o \
	$(BUILDDIR)/squake/vid_svgalib.o \
	$(BUILDDIR)/squake/snd_dma.o \
	$(BUILDDIR)/squake/snd_mem.o \
	$(BUILDDIR)/squake/snd_mix.o \
	$(BUILDDIR)/squake/snd_linux.o \
	\
	$(BUILDDIR)/squake/d_copy.o \
	$(BUILDDIR)/squake/d_draw.o \
	$(BUILDDIR)/squake/d_draw16.o \
	$(BUILDDIR)/squake/d_parta.o \
	$(BUILDDIR)/squake/d_polysa.o \
	$(BUILDDIR)/squake/d_scana.o \
	$(BUILDDIR)/squake/d_spr8.o \
	$(BUILDDIR)/squake/d_varsa.o \
	$(BUILDDIR)/squake/math.o \
	$(BUILDDIR)/squake/r_aliasa.o \
	$(BUILDDIR)/squake/r_drawa.o \
	$(BUILDDIR)/squake/r_edgea.o \
	$(BUILDDIR)/squake/r_varsa.o \
	$(BUILDDIR)/squake/surf16.o \
	$(BUILDDIR)/squake/surf8.o \
	$(BUILDDIR)/squake/worlda.o \
	$(BUILDDIR)/squake/r_aclipa.o \
	$(BUILDDIR)/squake/snd_mixa.o \
	$(BUILDDIR)/squake/sys_dosa.o

$(BUILDDIR)/bin/squake : $(SQUAKE_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SQUAKE_OBJS) $(SVGALDFLAGS) $(LDFLAGS)

####

$(BUILDDIR)/squake/cl_demo.o :  $(MOUNT_DIR)/cl_demo.c
	$(DO_CC)

$(BUILDDIR)/squake/cl_input.o : $(MOUNT_DIR)/cl_input.c
	$(DO_CC)

$(BUILDDIR)/squake/cl_main.o :  $(MOUNT_DIR)/cl_main.c
	$(DO_CC)

$(BUILDDIR)/squake/cl_parse.o : $(MOUNT_DIR)/cl_parse.c
	$(DO_CC)

$(BUILDDIR)/squake/cl_tent.o :  $(MOUNT_DIR)/cl_tent.c
	$(DO_CC)

$(BUILDDIR)/squake/chase.o :    $(MOUNT_DIR)/chase.c
	$(DO_CC)

$(BUILDDIR)/squake/cmd.o :      $(MOUNT_DIR)/cmd.c
	$(DO_CC)

$(BUILDDIR)/squake/common.o :   $(MOUNT_DIR)/common.c
	$(DO_DEBUG_CC)

$(BUILDDIR)/squake/console.o :  $(MOUNT_DIR)/console.c
	$(DO_CC)

$(BUILDDIR)/squake/crc.o :      $(MOUNT_DIR)/crc.c
	$(DO_CC)

$(BUILDDIR)/squake/cvar.o :     $(MOUNT_DIR)/cvar.c
	$(DO_CC)

$(BUILDDIR)/squake/draw.o :     $(MOUNT_DIR)/draw.c
	$(DO_CC)

$(BUILDDIR)/squake/d_edge.o :   $(MOUNT_DIR)/d_edge.c
	$(DO_CC)

$(BUILDDIR)/squake/d_fill.o :   $(MOUNT_DIR)/d_fill.c
	$(DO_CC)

$(BUILDDIR)/squake/d_init.o :   $(MOUNT_DIR)/d_init.c
	$(DO_CC)

$(BUILDDIR)/squake/d_modech.o : $(MOUNT_DIR)/d_modech.c
	$(DO_CC)

$(BUILDDIR)/squake/d_part.o :   $(MOUNT_DIR)/d_part.c
	$(DO_CC)

$(BUILDDIR)/squake/d_polyse.o : $(MOUNT_DIR)/d_polyse.c
	$(DO_CC)

$(BUILDDIR)/squake/d_scan.o :   $(MOUNT_DIR)/d_scan.c
	$(DO_CC)

$(BUILDDIR)/squake/d_sky.o :    $(MOUNT_DIR)/d_sky.c
	$(DO_CC)

$(BUILDDIR)/squake/d_sprite.o : $(MOUNT_DIR)/d_sprite.c
	$(DO_CC)

$(BUILDDIR)/squake/d_surf.o :   $(MOUNT_DIR)/d_surf.c
	$(DO_CC)

$(BUILDDIR)/squake/d_vars.o :   $(MOUNT_DIR)/d_vars.c
	$(DO_CC)

$(BUILDDIR)/squake/d_zpoint.o : $(MOUNT_DIR)/d_zpoint.c
	$(DO_CC)

$(BUILDDIR)/squake/host.o :     $(MOUNT_DIR)/host.c
	$(DO_CC)

$(BUILDDIR)/squake/host_cmd.o : $(MOUNT_DIR)/host_cmd.c
	$(DO_CC)

$(BUILDDIR)/squake/keys.o :     $(MOUNT_DIR)/keys.c
	$(DO_CC)

$(BUILDDIR)/squake/menu.o :     $(MOUNT_DIR)/menu.c
	$(DO_CC)

$(BUILDDIR)/squake/mathlib.o :  $(MOUNT_DIR)/mathlib.c
	$(DO_CC)

$(BUILDDIR)/squake/model.o :    $(MOUNT_DIR)/model.c
	$(DO_CC)

$(BUILDDIR)/squake/net_dgrm.o : $(MOUNT_DIR)/net_dgrm.c
	$(DO_CC)

$(BUILDDIR)/squake/net_loop.o : $(MOUNT_DIR)/net_loop.c
	$(DO_CC)

$(BUILDDIR)/squake/net_main.o : $(MOUNT_DIR)/net_main.c
	$(DO_CC)

$(BUILDDIR)/squake/net_vcr.o :  $(MOUNT_DIR)/net_vcr.c
	$(DO_CC)

$(BUILDDIR)/squake/net_udp.o :  $(MOUNT_DIR)/net_udp.c
	$(DO_CC)

$(BUILDDIR)/squake/net_bsd.o :  $(MOUNT_DIR)/net_bsd.c
	$(DO_CC)

$(BUILDDIR)/squake/nonintel.o : $(MOUNT_DIR)/nonintel.c
	$(DO_CC)

$(BUILDDIR)/squake/pr_cmds.o :  $(MOUNT_DIR)/pr_cmds.c
	$(DO_CC)

$(BUILDDIR)/squake/pr_edict.o : $(MOUNT_DIR)/pr_edict.c
	$(DO_CC)

$(BUILDDIR)/squake/pr_exec.o :  $(MOUNT_DIR)/pr_exec.c
	$(DO_CC)

$(BUILDDIR)/squake/r_aclip.o :  $(MOUNT_DIR)/r_aclip.c
	$(DO_CC)

$(BUILDDIR)/squake/r_alias.o :  $(MOUNT_DIR)/r_alias.c
	$(DO_CC)

$(BUILDDIR)/squake/r_bsp.o :    $(MOUNT_DIR)/r_bsp.c
	$(DO_CC)

$(BUILDDIR)/squake/r_light.o :  $(MOUNT_DIR)/r_light.c
	$(DO_CC)

$(BUILDDIR)/squake/r_draw.o :   $(MOUNT_DIR)/r_draw.c
	$(DO_CC)

$(BUILDDIR)/squake/r_efrag.o :  $(MOUNT_DIR)/r_efrag.c
	$(DO_CC)

$(BUILDDIR)/squake/r_edge.o :   $(MOUNT_DIR)/r_edge.c
	$(DO_CC)

$(BUILDDIR)/squake/r_misc.o :   $(MOUNT_DIR)/r_misc.c
	$(DO_CC)

$(BUILDDIR)/squake/r_main.o :   $(MOUNT_DIR)/r_main.c
	$(DO_CC)

$(BUILDDIR)/squake/r_sky.o :    $(MOUNT_DIR)/r_sky.c
	$(DO_CC)

$(BUILDDIR)/squake/r_sprite.o : $(MOUNT_DIR)/r_sprite.c
	$(DO_CC)

$(BUILDDIR)/squake/r_surf.o :   $(MOUNT_DIR)/r_surf.c
	$(DO_CC)

$(BUILDDIR)/squake/r_part.o :   $(MOUNT_DIR)/r_part.c
	$(DO_CC)

$(BUILDDIR)/squake/r_vars.o :   $(MOUNT_DIR)/r_vars.c
	$(DO_CC)

$(BUILDDIR)/squake/screen.o :   $(MOUNT_DIR)/screen.c
	$(DO_CC)

$(BUILDDIR)/squake/sbar.o :     $(MOUNT_DIR)/sbar.c
	$(DO_CC)

$(BUILDDIR)/squake/sv_main.o :  $(MOUNT_DIR)/sv_main.c
	$(DO_CC)

$(BUILDDIR)/squake/sv_phys.o :  $(MOUNT_DIR)/sv_phys.c
	$(DO_CC)

$(BUILDDIR)/squake/sv_move.o :  $(MOUNT_DIR)/sv_move.c
	$(DO_CC)

$(BUILDDIR)/squake/sv_user.o :  $(MOUNT_DIR)/sv_user.c
	$(DO_CC)

$(BUILDDIR)/squake/zone.o	:   $(MOUNT_DIR)/zone.c
	$(DO_CC)

$(BUILDDIR)/squake/view.o	:   $(MOUNT_DIR)/view.c
	$(DO_CC)

$(BUILDDIR)/squake/wad.o :      $(MOUNT_DIR)/wad.c
	$(DO_CC)

$(BUILDDIR)/squake/world.o :    $(MOUNT_DIR)/world.c
	$(DO_CC)

$(BUILDDIR)/squake/cd_linux.o : $(MOUNT_DIR)/cd_linux.c
	$(DO_CC)

$(BUILDDIR)/squake/sys_linux.o :$(MOUNT_DIR)/sys_linux.c
	$(DO_CC)

$(BUILDDIR)/squake/vid_svgalib.o:$(MOUNT_DIR)/vid_svgalib.c
	$(DO_O_CC)

$(BUILDDIR)/squake/snd_dma.o :  $(MOUNT_DIR)/snd_dma.c
	$(DO_CC)

$(BUILDDIR)/squake/snd_mem.o :  $(MOUNT_DIR)/snd_mem.c
	$(DO_CC)

$(BUILDDIR)/squake/snd_mix.o :  $(MOUNT_DIR)/snd_mix.c
	$(DO_CC)

$(BUILDDIR)/squake/snd_linux.o :$(MOUNT_DIR)/snd_linux.c
	$(DO_CC)

#####

$(BUILDDIR)/squake/d_copy.o :   $(MOUNT_DIR)/d_copy.s
	$(DO_AS)

$(BUILDDIR)/squake/d_draw.o :   $(MOUNT_DIR)/d_draw.s
	$(DO_AS)

$(BUILDDIR)/squake/d_draw16.o : $(MOUNT_DIR)/d_draw16.s
	$(DO_AS)

$(BUILDDIR)/squake/d_parta.o :  $(MOUNT_DIR)/d_parta.s
	$(DO_AS)

$(BUILDDIR)/squake/d_polysa.o : $(MOUNT_DIR)/d_polysa.s
	$(DO_AS)

$(BUILDDIR)/squake/d_scana.o :  $(MOUNT_DIR)/d_scana.s
	$(DO_AS)

$(BUILDDIR)/squake/d_spr8.o :   $(MOUNT_DIR)/d_spr8.s
	$(DO_AS)

$(BUILDDIR)/squake/d_varsa.o :  $(MOUNT_DIR)/d_varsa.s
	$(DO_AS)

$(BUILDDIR)/squake/math.o :     $(MOUNT_DIR)/math.s
	$(DO_AS)

$(BUILDDIR)/squake/r_aliasa.o : $(MOUNT_DIR)/r_aliasa.s
	$(DO_AS)

$(BUILDDIR)/squake/r_drawa.o :  $(MOUNT_DIR)/r_drawa.s
	$(DO_AS)

$(BUILDDIR)/squake/r_edgea.o :  $(MOUNT_DIR)/r_edgea.s
	$(DO_AS)

$(BUILDDIR)/squake/r_varsa.o :  $(MOUNT_DIR)/r_varsa.s
	$(DO_AS)

$(BUILDDIR)/squake/surf16.o :   $(MOUNT_DIR)/surf16.s
	$(DO_AS)

$(BUILDDIR)/squake/surf8.o :    $(MOUNT_DIR)/surf8.s
	$(DO_AS)

$(BUILDDIR)/squake/worlda.o :   $(MOUNT_DIR)/worlda.s
	$(DO_AS)

$(BUILDDIR)/squake/r_aclipa.o : $(MOUNT_DIR)/r_aclipa.s
	$(DO_AS)

$(BUILDDIR)/squake/snd_mixa.o : $(MOUNT_DIR)/snd_mixa.s
	$(DO_AS)

$(BUILDDIR)/squake/sys_dosa.o : $(MOUNT_DIR)/sys_dosa.s
	$(DO_AS)

#############################################################################
# MISC
#############################################################################

clean: clean-debug clean-release
	rm -f squake.spec glquake.spec quake.x11.spec

clean-debug:
	$(MAKE) clean2 BUILDDIR=$(BUILD_DEBUG_DIR) CFLAGS="$(DEBUG_CFLAGS)"

clean-release:
	$(MAKE) clean2 BUILDDIR=$(BUILD_RELEASE_DIR) CFLAGS="$(DEBUG_CFLAGS)"

clean2:
	-rm -f $(SQUAKE_OBJS) $(X11_OBJS) $(GLQUAKE_OBJS) $(GLSVGA_OBJS) \
		$(GLX_OBJS)

