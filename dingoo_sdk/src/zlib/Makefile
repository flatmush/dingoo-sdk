# Define the applications properties here:

LIB_NAME = z



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
INCLUDE   = -I. -I$(DINGOO_SDK)/include
DEFINES   = -DLIBZ_NO_STRERROR=1

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



SRC_H = zlib.h zconf.h

# Find all source files

SRC_C   = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.c))
OBJ     = $(notdir $(patsubst %.c, %.o, $(SRC_C)))



# Compile rules.

.PHONY : all

all : $(LIB_PATH)/lib$(LIB_NAME).a



$(LIB_PATH)/lib$(LIB_NAME).a : $(OBJ)
	$(AR) rcs $(LIB_PATH)/lib$(LIB_NAME).a $^
	make headers

$(OBJ) : %.o : %.c
	$(CC) $(CC_OPTS) -o $@ $<



# Clean rules

.PHONY : clean

clean :
	rm -f $(OBJ)
	rm -f $(LIB_PATH)/lib$(LIB_NAME).a
	rm -rf $(DINGOO_SDK)/include/zlib



# Header rules

.PHONY: headers

headers:
	rm -rf $(DINGOO_SDK)/include/zlib
	mkdir $(DINGOO_SDK)/include/zlib
	cp $(SRC_H) $(DINGOO_SDK)/include/zlib
