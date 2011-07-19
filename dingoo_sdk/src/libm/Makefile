# Define the applications properties here:

LIB_NAME = m



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
INCLUDE   = -I. -I./include -I$(DINGOO_SDK)/include
DEFINES   = -D_IEEE_LIBM

ifeq ($(platform),CC1800)
	_DEFINES  = -DMPU_CC1800 $(DEFINES)
	W_OPTS    = -Wall -Wextra -fno-pic -pipe -O2 -finline-functions -fomit-frame-pointer -msoft-float -fno-builtin -mcpu=arm926ej-s -mabi=apcs-gnu -mfpu=fpa -fno-strict-aliasing
	CPP_OPTS  =	-c -g -O2 -fno-exceptions -fno-rtti $(INCLUDE) $(W_OPTS) $(_DEFINES)
	CC_OPTS   = -c -g -O2 $(INCLUDE) $(W_OPTS) $(_DEFINES)
	CC_OPTS_A = $(CC_OPTS) -D_ASSEMBLER_

	LIB_PATH  = $(DINGOO_SDK)/lib/cc1800
else
	_DEFINES  = -D_DEBUG -DMPU_JZ4740 $(DEFINES)
	W_OPTS    = -Wall -Wextra -mips32 -mno-abicalls -fno-pic -finline-functions -fomit-frame-pointer -msoft-float -fno-builtin -fno-strict-aliasing
	CPP_OPTS  = -c -G0 -O3 -fno-exceptions -fno-rtti $(INCLUDE) $(W_OPTS) $(_DEFINES)
	CC_OPTS   = -c -G0 -O3 $(INCLUDE) $(W_OPTS) $(_DEFINES)
	CC_OPTS_A = $(CC_OPTS) -D_ASSEMBLER_

	LIB_PATH  = $(DINGOO_SDK)/lib
endif



# Find all source files

SRC_CPP = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.cpp))
SRC_C   = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.c))
SRC_S   = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.S))
OBJ_CPP = $(notdir $(patsubst %.cpp, %.o, $(SRC_CPP)))
OBJ_C   = $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJ_S   = $(notdir $(patsubst %.S, %.o, $(SRC_S)))
OBJ     = $(OBJ_CPP) $(OBJ_C) $(OBJ_S)



# Compile rules.

.PHONY : all

all : $(LIB_PATH)/lib$(LIB_NAME).a

$(LIB_PATH)/lib$(LIB_NAME).a : $(OBJ)
	$(AR) rcs $(LIB_PATH)/lib$(LIB_NAME).a *.o
	make headers

$(OBJ_CPP) : %.o : %.cpp
	$(CPP) $(CPP_OPTS) -o $@ $<

$(OBJ_C) : %.o : %.c
	$(CC) $(CC_OPTS) -o $@ $<

$(OBJ_S) : %.o : %.S
	$(CC) $(CC_OPTS_A) -o $@ $<



# Clean rules

.PHONY : clean

clean :
	rm -f $(OBJ)
	rm -f $(LIB_PATH)/lib$(LIB_NAME).a
	-rm -f $(DINGOO_SDK)/include/math.h



# Header rules

.PHONY: headers

headers:
	mkdir -p $(DINGOO_SDK)/include
	cp -f math.h $(DINGOO_SDK)/include
