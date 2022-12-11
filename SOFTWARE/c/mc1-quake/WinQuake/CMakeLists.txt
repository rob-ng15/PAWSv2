# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

cmake_minimum_required(VERSION 3.10)

project(mc1-quake LANGUAGES C CXX ASM)

set(SRCS
    chase.c
    cl_demo.c
    cl_input.c
    cl_main.c
    cl_parse.c
    cl_tent.c
    cmd.c
    common.c
    console.c
    crc.c
    cvar.c
    draw.c
    d_edge.c
    d_fill.c
    d_init.c
    d_modech.c
    d_part.c
    d_polyse.c
    d_scan.c
    d_sky.c
    d_sprite.c
    d_surf.c
    d_vars.c
    d_zpoint.c
    host.c
    host_cmd.c
    keys.c
    menu.c
    mathlib.c
    model.c
    nonintel.c
    pr_cmds.c
    pr_edict.c
    pr_exec.c
    r_aclip.c
    r_alias.c
    r_bsp.c
    r_light.c
    r_draw.c
    r_efrag.c
    r_edge.c
    r_misc.c
    r_main.c
    r_sky.c
    r_sprite.c
    r_surf.c
    r_part.c
    r_vars.c
    screen.c
    sbar.c
    sv_main.c
    sv_phys.c
    sv_move.c
    sv_user.c
    zone.c
    view.c
    wad.c
    world.c
    cd_null.c
    net_loop.c
    net_main.c
    net_vcr.c
)

if(MC1)
  list(APPEND DEFS -DMC1)
  list(APPEND OPTS -mcmodel=small)
  list(APPEND LIBS
       m
  )
  if(MC1SDK_HOME)
    message("Building for running on an MC1 computer.")
    list(APPEND DEFS
         -DMC1_SDK
    )
    list(APPEND OPTS
         "-I${MC1SDK_HOME}/include"
    )
    list(APPEND LIBS
         "-L${MC1SDK_HOME}/lib"
         -mno-crt0
         -lmc1crt0-app
         -lmc1
         "-T app-xram.ld"
    )
  else()
    message("Building for running in the MRISC32 simulator.")
    list(APPEND LIBS
         -msim
    )
  endif()
  list(APPEND SRCS
       in_mc1.c
       net_none.c
       snd_null.c
       sys_mc1.c
       vid_mc1.c
       d_scan_mr32.s
       r_surf_mr32.s
  )
elseif(UNIX)
  list(APPEND SRCS
       net_dgrm.c
       net_udp.c
       net_bsd.c
       snd_null.c
       sys_linux.c
  )
  list(APPEND LIBS m)

  find_package(SDL2)
  if(SDL2_FOUND)
    list(APPEND SRCS vid_sdl2.c)
    list(APPEND LIBS ${SDL2_LIBRARIES})
  else()
    find_package(X11)
    if(X11_FOUND)
      list(APPEND DEFS -DX11)
      list(APPEND SRCS vid_x.c)
      list(APPEND LIBS ${X11_LIBRARIES})
    else()
      list(APPEND SRCS vid_null.c)
    endif()
  endif()
else()
  message("Unknown system - using null implementation")
  list(APPEND SRCS
       in_null.c
       net_none.c
       snd_null.c
       sys_null.c
       vid_null.c
  )
  list(APPEND LIBS m)
endif()

# Always warn for unnecessary use of double-precision.
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  list(APPEND OPTS
       -Wdouble-promotion)
endif()

set(SANITIZE OFF)

if(SANITIZE)
  # Enable sanitizers in debug builds.
  if((NOT MC1) AND (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang"))
    list(APPEND SANITIZERS
         $<$<CONFIG:Debug>:-fsanitize=address>
         $<$<CONFIG:Debug>:-fsanitize=null>
         $<$<CONFIG:Debug>:-fsanitize=pointer-overflow>
         $<$<CONFIG:Debug>:-fsanitize=undefined>)
  endif()

  # More warnings.
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    list(APPEND OPTS
         -pedantic
         -W
         -Wall
         -Wextra)
  endif()
endif()

add_executable(mc1quake ${SRCS})
target_compile_definitions(mc1quake PRIVATE ${DEFS})
target_compile_options(mc1quake PRIVATE ${OPTS} ${SANITIZERS})
target_link_libraries(mc1quake PRIVATE ${LIBS} ${SANITIZERS})
set_property(TARGET mc1quake PROPERTY C_STANDARD 11)
set_property(TARGET mc1quake PROPERTY C_EXTENSIONS OFF)

