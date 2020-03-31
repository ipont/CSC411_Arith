#ifndef IMAGEIO_DEFINED
#define IMAGEIO_DEFINED

#include <stdio.h>
#include "structures.h"
#include "pnmrdr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bitpack.h"

void* readCompressed(FILE* f);

Pnm_ppm* readUncompressed(FILE* f);

void writeCompressed(FILE* f, Pnm_ppm* pixmap);

void writeUncompressed(FILE* f, Pnm_ppm* pixmap);

#endif
