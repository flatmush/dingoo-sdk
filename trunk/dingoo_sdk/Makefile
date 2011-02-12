# Core/Extra definitions

CORE_LIBS = \
	jz4740\
	libc\
	libstdc++\
	sml\
	smlgui

EXTRA_LIBS = \
	libm\
	modplay\
	zlib\
	libpng\
	freetype\
	tremor\
	libmikmod/dingoo\
	libmad\
	SDL

CORE_SAMPLES = \
	AstroLander\
	fgl_cube_demo\
	fgl_model_demo\
	fgl_sphere_demo\
	fgl_sprite_demo\
	ListDirectory\
	MineSweeper\
	posix_readdir\
	printf_replacement\
	sml_audio_demo\
	sml_mixer_demo\
	sml_overclock\
	sml_sprite_demo\
	smlgui_keyboard\
	tga_viewer_sim\
	zaxxon_hello_world

EXTRA_SAMPLES = \
	modplay_demo\
	sdl_hello_world\
	SDLPong



# Default make

.PHONY : all
all:
	make core
	make extra
	make samples



# Install rules

.PHONY: install
install:
	make install_extra_libs

.PHONY: install_full
	make install_extra

.PHONY: install_quick
install_quick:
	make core_headers
	make core_libs

.PHONY: install_core
install_core:
	make clean_core
	make core_headers
	make core

.PHONY: install_extra
install_extra:
	make install_core
	make clean_extra
	make extra_headers
	make extra
	make test

.PHONY: install_core_libs
install_core_libs:
	make clean_core_libs
	make core_headers
	make core_libs

.PHONY: install_extra_libs
install_extra_libs:
	make install_core_libs
	make clean_extra_libs
	make extra_headers
	make extra_libs



# Make rules

.PHONY: core
core:
	make core_libs
	make core_samples

.PHONY: extra
extra:
	make extra_libs
	make extra_samples

.PHONY: libs
libs:
	make core_libs
	make extra_libs

.PHONY: core_libs
core_libs:
	$(foreach LIB, $(CORE_LIBS), make -C $(DINGOO_SDK)/src/$(LIB);)

.PHONY: extra_libs
extra_libs:
	$(foreach LIB, $(EXTRA_LIBS), make -C $(DINGOO_SDK)/src/$(LIB);)

.PHONY: samples
samples:
	make core_samples
	make extra_samples

.phony: core_samples
core_samples:
	$(foreach SAMPLE, $(CORE_SAMPLES), make -C $(DINGOO_SDK)/samples/$(SAMPLE);)

.phony: extra_samples
extra_samples:
	$(foreach SAMPLE, $(EXTRA_SAMPLES), make -C $(DINGOO_SDK)/samples/$(SAMPLE);)

.PHONY: test
test:
	make -C $(DINGOO_SDK)/test


# Clean rules

.PHONY: clean
clean:
	make clean_test
	make clean_samples
	make clean_libs

.PHONY: clean_core
clean_core:
	make clean_core_samples
	make clean_core_libs
	make clean_test

.PHONY: clean_extra
clean_extra:
	make clean_extra_samples
	make clean_extra_libs

.PHONY: clean_libs
clean_libs:
	make clean_core_libs
	make clean_extra_libs

.PHONY: clean_core_libs
clean_core_libs:
	$(foreach LIB, $(CORE_LIBS), make -C $(DINGOO_SDK)/src/$(LIB) clean;)

.PHONY: clean_extra_libs
clean_extra_libs:
	$(foreach LIB, $(EXTRA_LIBS), make -C $(DINGOO_SDK)/src/$(LIB) clean;)

.PHONY: clean_samples
clean_samples:
	make clean_core_samples
	make clean_extra_samples

.PHONY: clean_core_samples
clean_core_samples:
	$(foreach SAMPLE, $(CORE_SAMPLES), make -C $(DINGOO_SDK)/samples/$(SAMPLE) clean;)

.phony: clean_extra_samples
clean_extra_samples:
	$(foreach SAMPLE, $(EXTRA_SAMPLES), make -C $(DINGOO_SDK)/samples/$(SAMPLE) clean;)

.PHONY: clean_test
clean_test:
	make -C $(DINGOO_SDK)/test clean



# Header rules

.PHONY: headers
headers:
	make core_headers
	make extra_headers

.PHONY: core_headers
core_headers:
	$(foreach LIB, $(CORE_LIBS), make -C $(DINGOO_SDK)/src/$(LIB) headers;)

.PHONY: extra_headers
extra_headers:
	$(foreach LIB, $(EXTRA_LIBS), make -C $(DINGOO_SDK)/src/$(LIB) headers;)


.PHONY: tools
tools:
	rm -f $(DINGOO_SDK)/tools/elf2app/elf2app
	cc $(DINGOO_SDK)/tools/elf2app/src/*.c -o $(DINGOO_SDK)/tools/elf2app/elf2app
	rm -f $(DINGOO_SDK)/tools/bin2h/bin2h
	cc $(DINGOO_SDK)/tools/bin2h/src/bin2h.c -o $(DINGOO_SDK)/tools/bin2h/bin2h

.PHONY: documentation
documentation:
	$(DINGOO_SDK)/src/Doxyfile
