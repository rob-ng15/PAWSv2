OS := $(shell uname -s)
IS_APPLE := $(shell echo $(OS)|grep -i darwin)

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

main.o : shell/main.c core/my_basic.h my_basic.o
	$(CC) $(CFLAGS) -c shell/main.c -o build/$@ -Wno-unused-result

my_basic.o : core/my_basic.c core/my_basic.h
	$(CC) $(CFLAGS) -c core/my_basic.c -o build/$@ -Wno-multichar -Wno-overflow -Wno-unused-result

clean :
	rm -f build/main.o build/my_basic.o build/libPAWS.o build/crt0.o
