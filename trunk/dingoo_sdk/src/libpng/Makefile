# Define the applications properties here:

LIB_NAME = png

# Define the compiler settings here:

CPP       = mipsel-linux-g++
CC        = mipsel-linux-gcc
AR        = mipsel-linux-ar

SOURCE    = .

INCLUDE   = -I. -I$(DINGOO_SDK)/include -I$(DINGOO_SDK)/include/zlib

W_OPTS    = -Wall -Wextra -finline-functions -fomit-frame-pointer -msoft-float -fno-builtin -fno-exceptions
CPP_OPTS  = -G0 -O3 $(INCLUDE) -mips32 -mno-abicalls -fno-pic $(W_OPTS) -D_DEBUG -DMPU_JZ4740 -c
CC_OPTS   = -G0 -O3 $(INCLUDE) -mips32 -mno-abicalls -fno-pic $(W_OPTS) -D_DEBUG -DMPU_JZ4740 -c
CC_OPTS_A = $(CC_OPTS) -D_ASSEMBLER_

LIB_PATH  = $(DINGOO_SDK)/lib

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

