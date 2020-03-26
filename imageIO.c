#include "imageIO.h"

void* readCompressed(FILE* f){
	assert(f != NULL);
}

void* readUncompressed(FILE* f){
	assert(f != NULL);
	Pnm_ppm* img = malloc(sizeof(Pnm_ppm));
	
	Pnmrdr_T pgmReader = Pnmrdr_new(input);
	Pnmrdr_mapdata pgmData = Pnmrdr_data(pgmReader);
	
	img->height = pgmData.height;
	img->width = pgmData.width;
	img->denominator = pgmData.denominator;
	img->pixels = malloc(sizeof(RgbPix) * img->height * img->width);
	
	for (int i = 0; i < img->width * img->height; i++){
		int r = (int) Pnmrdr_get(pgmReader);
		int g = (int) Pnmrdr_get(pgmReader);
		int b = (int) Pnmrdr_get(pgmReader);
		img->pixels[i] = {r, g, b};
	}
}

void writeCompressed(FILE* f, Pnm_ppm* pixmap);

void writeUncompressed(FILE* f, Pnm_ppm* pixmap);
