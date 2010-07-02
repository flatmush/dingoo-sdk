# Define the applications properties here:

LIB_NAME = freetype



# Define the compiler settings here:

CPP       = mipsel-linux-g++
CC        = mipsel-linux-gcc
AR        = mipsel-linux-ar

INCLUDE   = -I. -I./include -I$(DINGOO_SDK)/include

W_OPTS    = -Wall -Wextra -finline-functions -fomit-frame-pointer -msoft-float -fno-builtin -fno-exceptions
CPP_OPTS  = -G0 -O3 $(INCLUDE) -DFT2_BUILD_LIBRARY -mips32 -mno-abicalls -fno-pic $(W_OPTS) -D_DEBUG -DMPU_JZ4740 -c
CC_OPTS   = -G0 -O3 $(INCLUDE) -DFT2_BUILD_LIBRARY -mips32 -mno-abicalls -fno-pic $(W_OPTS) -D_DEBUG -DMPU_JZ4740 -c
CC_OPTS_A = $(CC_OPTS) -D_ASSEMBLER_

LIB_PATH  = $(DINGOO_SDK)/lib



# Find all source files

FTBASE = ftbase.o ftbbox.o ftbdf.o ftbitmap.o ftcid.o   \
	  ftfstype.o ftgasp.o ftglyph.o ftgxval.o            \
	  ftlcdfil.o ftmm.o ftotval.o ftpatent.o ftpfr.o \
	  ftstroke.o ftsynth.o fttype1.o ftwinfnt.o          \
	  ftxf86.o ftsystem.o ftinit.o
	  
FTDEBUG = ftdebug.o

FTAFIT = autofit.o

FTGXV = gxvalid.o

FTOTV = otvalid.o

FTPS = psaux.o psnames.o pshinter.o

FTRASTER = raster.o smooth.o

#FTFONTD = cff.o type1.o type42.o type1cid.o truetype.o\
#	   bdf.o pcf.o pfr.o winfnt.o

FTFONTD = cff.o truetype.o

SRC_H   = include/*
OBJLIST = $(FTBASE) $(FTAFIT) $(FTGXV) $(FTOTV) $(FTPS) $(FTRASTER) sfnt.o ftcache.o $(FTFONTD)
OBJ     = $(foreach file, $(OBJLIST), objs/$(file))



# Compile rules.

.PHONY : all

all : $(LIB_PATH)/lib$(LIB_NAME).a



$(LIB_PATH)/lib$(LIB_NAME).a : $(OBJ)
#	$(AR) rcs $(LIB_PATH)/lib$(LIB_NAME).a *.o
	$(AR) rcs $(LIB_PATH)/lib$(LIB_NAME).a $^
	make headers



#
# FreeType2 library base
#
objs/ftbase.o: ./src/base/ftbase.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftinit.o: ./src/base/ftinit.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftsystem.o: ./src/base/ftsystem.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftdebug.o: ./src/base/ftdebug.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library base extensions
#
objs/ftbbox.o: ./src/base/ftbbox.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftbdf.o: ./src/base/ftbdf.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftbitmap.o: ./src/base/ftbitmap.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftcid.o: ./src/base/ftcid.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftfstype.o: ./src/base/ftfstype.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftgasp.o: ./src/base/ftgasp.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftglyph.o: ./src/base/ftglyph.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftgxval.o: ./src/base/ftgxval.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftlcdfil.o: ./src/base/ftlcdfil.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftmm.o: ./src/base/ftmm.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftotval.o: ./src/base/ftotval.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftpatent.o: ./src/base/ftpatent.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftpfr.o: ./src/base/ftpfr.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftstroke.o: ./src/base/ftstroke.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftsynth.o: ./src/base/ftsynth.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/fttype1.o: ./src/base/fttype1.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftwinfnt.o: ./src/base/ftwinfnt.c
	$(CC) $(CC_OPTS) -o $@ $<

objs/ftxf86.o: ./src/base/ftxf86.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library autofitting module
#
objs/autofit.o: ./src/autofit/autofit.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library postscript hinting module
#
objs/pshinter.o: ./src/pshinter/pshinter.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library PS support module
#
objs/psaux.o: ./src/psaux/psaux.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library PS glyph names module
#
objs/psnames.o: ./src/psnames/psnames.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library monochrome raster module
#
objs/raster.o: ./src/raster/raster.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library anti-aliasing raster module
#
objs/smooth.o: ./src/smooth/smooth.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library 'sfnt' module
#
objs/sfnt.o: ./src/sfnt/sfnt.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library glyph and image caching system
#
objs/ftcache.o: ./src/cache/ftcache.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library OpenType font driver
#
objs/cff.o: ./src/cff/cff.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library TrueType font driver
#
objs/truetype.o: ./src/truetype/truetype.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library Type1 font driver
#
#objs/type1.o: ./src/type1/type1.c
#	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library Type42 font driver
#
#objs/type42.o: ./src/type42/type42.c
#	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library CID-keyed Type1 font driver
#
#objs/type1cid.o: ./src/cid/type1cid.c
#	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library BDF bitmap font driver
#
#objs/bdf.o: ./src/bdf/bdf.c
#	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library PCF bitmap font driver
#
#objs/pcf.o: ./src/pcf/pcf.c
#	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library gzip support for compressed PCF bitmap fonts
#
#objs/gzip.o: ./src/gzip/ftgzip.c
#	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library compress support for compressed PCF bitmap fonts
#
#objs/lzw.o: ./src/lzw/ftlzw.c
#	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library PFR font driver
#
#objs/pfr.o: ./src/pfr/pfr.c
#	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library Windows FNT/FON bitmap font driver
#
#objs/winfnt.o: ./src/winfonts/winfnt.c
#	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library TrueTypeGX Validator
#
objs/gxvalid.o: ./src/gxvalid/gxvalid.c
	$(CC) $(CC_OPTS) -o $@ $<

#
# FreeType2 library OpenType validator
#
objs/otvalid.o: ./src/otvalid/otvalid.c
	$(CC) $(CC_OPTS) -o $@ $<



# Clean rules

.PHONY : clean

clean :
	rm -f $(OBJ)
	rm -f $(LIB_PATH)/lib$(LIB_NAME).a
	rm -rf $(DINGOO_SDK)/include/$(LIB_NAME)
	rm -f $(DINGOO_SDK)/include/ft2build.h



# Header rules

.PHONY: headers

headers:
	rm -rf $(DINGOO_SDK)/include/$(LIB_NAME)
	rm -f $(DINGOO_SDK)/include/ft2build.h
	mkdir $(DINGOO_SDK)/include/$(LIB_NAME)
	cp -r $(SRC_H) $(DINGOO_SDK)/include/
