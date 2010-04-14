#ifndef __fgl_image_tga_h__
#define __fgl_image_tga_h__

#include "fgl_image.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

extern fgl_image_complex* fgl_image_load_tga(FILE* inStream);

#endif
