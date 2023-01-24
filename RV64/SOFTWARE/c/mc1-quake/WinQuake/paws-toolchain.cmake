# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

# This file is a fairly generic CMake toolchain file for the MC1 computer.
# To use it, run CMake with the argument:
#   -DCMAKE_TOOLCHAIN_FILE=mc1-toolchain.cmake

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)
set(PAWS TRUE)

set(CMAKE_C_COMPILER riscv64-elf-gcc)
set(CMAKE_CXX_COMPILER riscv64-elf-g++)
set(CMAKE_OBJCOPY riscv64-elf-objcopy)

set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
