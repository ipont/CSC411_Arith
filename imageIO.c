#include "imageIO.h"
#include <stdint.h>
#include <stdio.h>

static inline uint64_t readWord(FILE* f){

	uint64_t bWord = 0;
	bWord = Bitpack_newu (bWord, 8, 24, getc(f));
	bWord = Bitpack_newu (bWord, 8, 16, getc(f));
	bWord = Bitpack_newu (bWord, 8, 8, getc(f));
	bWord = Bitpack_newu (bWord, 8, 0, getc(f));
	
	return bWord;
}

static inline Word makeWord(uint64_t bWord){
	
	uint64_t a  = Bitpack_getu(bWord, 9, 23);
	int64_t b = Bitpack_gets(bWord, 5, 18);
	int64_t c = Bitpack_gets(bWord, 5, 13);
	int64_t d = Bitpack_gets(bWord, 5, 8);
	unsigned pb = Bitpack_getu(bWord, 4, 4);
	unsigned pr = Bitpack_getu(bWord, 4, 0);

	return  (Word){a, b, c, d, pb, pr};
}

void* readCompressed(FILE* f){
	
	unsigned height, width;
	int read = fscanf(f, "Compressed image format 2\n%u %u", &width, &height);
	assert(read == 2);
	int c = getc(f);
	assert(c == '\n');

	int blocks = (height * width) / 4;
	Pnm_ppm* img = malloc(sizeof(Pnm_ppm));
	img->pixels = malloc(sizeof(Word) * blocks);

	img->height = height;
	img->width = width;
	img->denominator = 255; 

	// value	Type			width	lsb
	// a 		Unsigned scaled integer 9 bits 	23
	// b 		Signed scaled integer 	5 bits 	18
	// c 		Signed scaled integer 	5 bits 	13
	// d 		Signed scaled integer 	5 bits 	8
	// index(PB) 	Unsigned index 		4 bits 	4
	// index(PR) 	Unsigned index 		4 bits 	0
	Word* cWord = img->pixels;

	for (int i = 0; i < blocks; i ++){
		uint64_t bWord = readWord(f);
		*(cWord + i) = makeWord(bWord);
	}

	return img;
}

Pnm_ppm* readUncompressed(FILE* f){
	// TODO fuck with the pixel array pointer stuff
	assert(f != NULL);
	Pnm_ppm* img = malloc(sizeof(Pnm_ppm));
	
	Pnmrdr_T pgmReader = Pnmrdr_new(f);
	Pnmrdr_mapdata pgmData = Pnmrdr_data(pgmReader);
	
	img->height = pgmData.height;
	img->width = pgmData.width;
	img->denominator = pgmData.denominator;

	if (img->width%2 != 0){
	       img->width -= 1;
	}
	if (img->height%2 != 0){
		img->height -= 1;
	}

	img->pixels = (void *)malloc(sizeof(RgbPix) * img->height * img->width);	

	for (int r = 0; r < img->height; r++){
		for (int c = 0; c < img->width; c++){

			RgbPix buf;
			buf.r = (float) Pnmrdr_get(pgmReader);
			buf.g = (float) Pnmrdr_get(pgmReader);
			buf.b = (float) Pnmrdr_get(pgmReader);
			*(((RgbPix *)img->pixels) + r * img->width + c) = buf;
		}
	}
	return img;
}

void writeCompressed(FILE* f, Pnm_ppm* pixmap);


// writes pnm image to stdout
void writeUncompressed(FILE* f, Pnm_ppm* pixmap){
	(void)f;
	// print header
	(void) fprintf(stdout, "P6\n%d %d\n%d\n", pixmap->width, pixmap->height, pixmap->denominator);
	//printf("P6\n");
	//printf("%d %d\n", pixmap->width, pixmap->height);
	RgbPix* cPix = pixmap->pixels;

	for (int i = 0; i < pixmap->height * pixmap->width; ++i){
		int r = (cPix + i)->r * pixmap->denominator;
		int g = (cPix + i)->g * pixmap->denominator;
		int b = (cPix + i)->b * pixmap->denominator;
		static unsigned char color[3];
		color[0] = r;
		color[1] = g;
		color[2] = b;
		(void)fwrite(color, 1, 3, stdout); 
		//printf("%d%d%d", r, g, b);
	}
	printf("\n");
}

