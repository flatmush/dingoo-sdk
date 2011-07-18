# Define the applications properties here:

LIB_NAME = SDL



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



SOURCE    = \
	. \
	./libc_ext \
	./src \
	./src/audio \
	./src/audio/dingoo \
	./src/cdrom \
	./src/cdrom/dummy \
	./src/cpuinfo \
	./src/events \
	./src/file \
	./src/joystick \
	./src/joystick/dummy \
	./src/loadso/dummy \
	./src/main/dummy \
	./src/stdlib \
	./src/timer \
	./src/timer/dummy \
	./src/timer/dingoo \
	./src/thread \
	./src/thread/generic \
	./src/video \
	./src/video/dummy \
	./src/video/slcd \
	./SDL_image \
	./SDL_ttf \
	./SDL_mixer \

INCLUDE   = -I. -I./include -I$(DINGOO_SDK)/include -I$(DINGOO_SDK)/include/png -I$(DINGOO_SDK)/include/zlib -I$(DINGOO_SDK)/include/mikmod
DEFINES   = -DWAV_MUSIC -DOGG_MUSIC -DOGG_USE_TREMOR -DMOD_MUSIC

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

SRC_CPP = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.cpp))
SRC_C   = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.c))
SRC_H_INC = $(foreach dir, $(SOURCE), $(wildcard $(dir)/include/*.h))
SRC_H_TTF = ./SDL_ttf/SDL_ttf.h
SRC_H_IMG = ./SDL_image/SDL_image.h
SRC_H_MIX = ./SDL_mixer/SDL_mixer.h
SRC_H   = $(SRC_H_INC) $(SRC_H_TTF) $(SRC_H_IMG) $(SRC_H_MIX)
SRC_S   = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.S))
OBJ_CPP = $(patsubst %.cpp, %.o, $(SRC_CPP))
OBJ_C   = $(patsubst %.c, %.o, $(SRC_C))
OBJ_S   = $(patsubst %.S, %.o, $(SRC_S))
OBJ     = $(OBJ_CPP) $(OBJ_C) $(OBJ_S)

# Debug
#$(warning $(OBJ_C))
#$(warning $(SRC_H))

# Compile rules.

.PHONY : all

all : $(LIB_PATH)/lib$(LIB_NAME).a

$(LIB_PATH)/lib$(LIB_NAME).a : $(OBJ)
#	$(AR) rcs $(LIB_PATH)/lib$(LIB_NAME).a *.o
	$(AR) rcs $(LIB_PATH)/lib$(LIB_NAME).a $^
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
	rm -rf $(DINGOO_SDK)/include/$(LIB_NAME)



# Header rules

.PHONY: headers

headers:
	rm -rf $(DINGOO_SDK)/include/$(LIB_NAME)
	mkdir $(DINGOO_SDK)/include/$(LIB_NAME)
	cp $(SRC_H) $(DINGOO_SDK)/include/$(LIB_NAME)
