include ../makefile-lto.inc

PROJECT=PFORTH
PROJECTTYPE=COMPUTER/PFORTH
PRJSRC=$(wildcard csrc/*.c) $(wildcard csrc/*.cpp) $(wildcard csrc/*.s) $(wildcard csrc/*.h)
PRJOBJ=csrc/pf_cglue.o csrc/pf_clib.o csrc/pf_core.o csrc/pf_inner.o \
		csrc/pf_io.o csrc/pf_main.o csrc/pf_mem.o csrc/pf_save.o \
		csrc/pf_text.o csrc/pf_words.o csrc/pfcompil.o csrc/pfcustom.o \
		csrc/pf_fileio_stdio.o csrc/pf_io_stdio.o

include ../makefile-paws.inc
