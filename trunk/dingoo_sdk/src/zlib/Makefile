# Define the applications properties here:

LIB_NAME = z



# Define the compiler settings here:

CPP       = mipsel-linux-g++
CC        = mipsel-linux-gcc
AR        = mipsel-linux-ar

SOURCE    = .

INCLUDE   = -I. -I$(DINGOO_SDK)/include

W_OPTS    = -Wall -Wextra -finline-functions -fomit-frame-pointer -msoft-float -fno-builtin -fno-exceptions
CPP_OPTS  = -G0 -O3 $(INCLUDE) -mips32 -mno-abicalls -fno-pic $(W_OPTS) -D_DEBUG -DMPU_JZ4740 -c
CC_OPTS   = -G0 -O3 $(INCLUDE) -mips32 -mno-abicalls -fno-pic $(W_OPTS) -D_DEBUG -DMPU_JZ4740 -c
CC_OPTS_A = $(CC_OPTS) -D_ASSEMBLER_

LIB_PATH  = $(DINGOO_SDK)/lib

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
