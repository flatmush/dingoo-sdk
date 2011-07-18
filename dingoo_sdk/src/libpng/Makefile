# Define the applications properties here:

LIB_NAME = png



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



SOURCE    = .
INCLUDE   = -I. -I$(DINGOO_SDK)/include -I$(DINGOO_SDK)/include/zlib
DEFINES   = 

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



SRC_H = png.h pngconf.h

OBJS = png.o pngset.o pngget.o pngrutil.o pngtrans.o pngwutil.o \
	pngread.o pngrio.o pngwio.o pngwrite.o pngrtran.o \
	pngwtran.o pngmem.o pngerror.o pngpread.o

# Compile rules.

.PHONY : all

all : $(LIB_PATH)/lib$(LIB_NAME).a

$(LIB_PATH)/lib$(LIB_NAME).a: $(OBJS)
	$(AR) rcs $(LIB_PATH)/lib$(LIB_NAME).a $^
	make headers

$(OBJS) : %.o : %.c
	$(CC) $(CC_OPTS) -o $@ $<



# Clean rules

.PHONY : clean

clean :
	rm -f $(OBJS)
	rm -f $(LIB_PATH)/lib$(LIB_NAME).a
	rm -rf $(DINGOO_SDK)/include/$(LIB_NAME)



# Header rules

.PHONY: headers

headers:
	rm -rf $(DINGOO_SDK)/include/$(LIB_NAME)
	mkdir $(DINGOO_SDK)/include/$(LIB_NAME)
	cp $(SRC_H) $(DINGOO_SDK)/include/$(LIB_NAME)

