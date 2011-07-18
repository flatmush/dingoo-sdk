# Define the applications properties here:

LIB_NAME = mikmod



# Define the compiler settings here:

ifeq ($(platform),CC1800)
	CPP       = arm-none-eabi-g++
	CC        = arm-none-eabi-gcc
	AR        = arm-none-eabi-ar
else
	CPP       = mipsel-linux-g++
	CC        = mipsel-linux-gcc
	AR        = mipsel-linux-ar
endif



INCLUDE   = -I. -I../include -I$(DINGOO_SDK)/include
DEFINES   = -DHAVE_CONFIG_H

ifeq ($(platform),CC1800)
	_DEFINES  = -DMPU_CC1800 $(DEFINES)
	W_OPTS    = -Wall -Wextra -fno-pic -pipe -O2 -finline-functions -fomit-frame-pointer -msoft-float -fno-builtin -mcpu=arm926ej-s -mabi=apcs-gnu -mfpu=fpa
	CPP_OPTS  =	-c -g -O2 -fno-exceptions -fno-rtti $(INCLUDE) $(W_OPTS) $(_DEFINES)
	CC_OPTS   = -c -g -O2 $(INCLUDE) $(W_OPTS) $(_DEFINES)
	CC_OPTS_A = $(CC_OPTS) -D_ASSEMBLER_

	LIB_PATH  = $(DINGOO_SDK)/lib/cc1800
else
	_DEFINES  = -D_DEBUG -DMPU_JZ4740 $(DEFINES)
	W_OPTS    = -Wall -Wextra -mips32 -mno-abicalls -fno-pic -finline-functions -fomit-frame-pointer -msoft-float -fno-builtin
	CPP_OPTS  = -c -G0 -O3 -fno-exceptions -fno-rtti $(INCLUDE) $(W_OPTS) $(_DEFINES)
	CC_OPTS   = -c -G0 -O3 $(INCLUDE) $(W_OPTS) $(_DEFINES)
	CC_OPTS_A = $(CC_OPTS) -D_ASSEMBLER_

	LIB_PATH  = $(DINGOO_SDK)/lib
endif



# Find all source files

OBJ= drv_nos.o \
    load_669.o load_amf.o load_dsm.o load_far.o load_gdm.o load_it.o  \
    load_imf.o load_m15.o load_med.o load_mod.o load_mtm.o load_okt.o \
    load_s3m.o load_stm.o load_stx.o load_ult.o load_uni.o load_xm.o \
    mmalloc.o mmerror.o mmio.o \
    mdriver.o mdreg.o mloader.o mlreg.o mlutil.o mplayer.o munitrk.o mwav.o \
    npertab.o sloader.o virtch.o virtch2.o virtch_common.o

# Compile rules.

.PHONY : all

all : $(LIB_PATH)/lib$(LIB_NAME).a

$(LIB_PATH)/lib$(LIB_NAME).a : $(OBJ)
	$(AR) rcs $(LIB_PATH)/lib$(LIB_NAME).a $^
	make headers



drv_nos.o:      ../drivers/drv_nos.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_669.o:     ../loaders/load_669.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_amf.o:     ../loaders/load_amf.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_dsm.o:     ../loaders/load_dsm.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_far.o:     ../loaders/load_far.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_gdm.o:     ../loaders/load_gdm.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_it.o:      ../loaders/load_it.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_imf.o:     ../loaders/load_imf.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_m15.o:     ../loaders/load_m15.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_med.o:     ../loaders/load_med.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_mod.o:     ../loaders/load_mod.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_mtm.o:     ../loaders/load_mtm.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_okt.o:     ../loaders/load_okt.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_s3m.o:     ../loaders/load_s3m.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_stm.o:     ../loaders/load_stm.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_stx.o:     ../loaders/load_stx.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_ult.o:     ../loaders/load_ult.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_uni.o:     ../loaders/load_uni.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
load_xm.o:      ../loaders/load_xm.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
mmalloc.o:      ../mmio/mmalloc.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
mmerror.o:      ../mmio/mmerror.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
mmio.o:         ../mmio/mmio.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
mdriver.o:      ../playercode/mdriver.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
mdreg.o:        ../playercode/mdreg.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
mloader.o:      ../playercode/mloader.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
mlreg.o:        ../playercode/mlreg.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
mlutil.o:       ../playercode/mlutil.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
mplayer.o:      ../playercode/mplayer.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
munitrk.o:      ../playercode/munitrk.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
mwav.o:         ../playercode/mwav.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
npertab.o:      ../playercode/npertab.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
sloader.o:      ../playercode/sloader.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
virtch.o:       ../playercode/virtch.c ../playercode/virtch_common.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
virtch2.o:      ../playercode/virtch2.c ../playercode/virtch_common.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<
virtch_common.o:        ../playercode/virtch_common.c \
	                ../dingoo/mikmod_build.h ../include/mikmod_internals.h
	$(CC) $(CC_OPTS) -o $@ $<



# Clean rules

.PHONY : clean

clean :
	rm -f $(OBJ)
	rm -f $(LIB_PATH)/lib$(LIB_NAME).a
	rm -rf $(DINGOO_SDK)/include/$(LIB_NAME)



# Header rules

.PHONY: headers

headers:
	rm -rf $(DINGOO_SDK)/include/$(LIB_NAME)
	mkdir $(DINGOO_SDK)/include/$(LIB_NAME)
	cp mikmod_build.h $(DINGOO_SDK)/include/$(LIB_NAME)/mikmod.h
