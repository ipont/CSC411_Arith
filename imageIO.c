#include "imageIO.h"
#include <stdint.h>
#include <stdio.h>

static inline uint64_t readWord(FILE* f){

	uint64_t bWord = 0;
	bWord = Bitpack_newu (bWord, 8, 24, (uint64_t)getc(f));
	bWord = Bitpack_newu (bWord, 8, 16, (uint64_t)getc(f));
	bWord = Bitpack_newu (bWord, 8, 8, (uint64_t)getc(f));
	bWord = Bitpack_newu (bWord, 8, 0, (uint64_t)getc(f));
	
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

static inline void writeWord(FILE* f, Word w){

	uint64_t bWord = Bitpack_newu(0, 9, 23, w.a);
	bWord = Bitpack_news(bWord, 5, 18, w.b);
	bWord = Bitpack_news(bWord, 5, 13, w.c);
	bWord = Bitpack_news(bWord, 5, 8, w.d);
	bWord = Bitpack_newu(bWord, 4, 4, w.pb);
	bWord = Bitpack_newu(bWord, 4, 0, w.pr);

	putc(Bitpack_getu(bWord, 8, 24), f);
	putc(Bitpack_getu(bWord, 8, 16), f);
	putc(Bitpack_getu(bWord, 8, 8), f);
	putc(Bitpack_getu(bWord, 8, 0), f);
	return;
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

	Word* cWord = img->pixels;

	for (int i = 0; i < blocks; i ++){
		uint64_t temp = readWord(f);
		*(cWord + i) = makeWord(temp);
	}

	return img;
}

Pnm_ppm* readUncompressed(FILE* f){
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
	Pnmrdr_free(&pgmReader);
	return img;
}

void writeCompressed(FILE* f, Pnm_ppm* pixmap){
	
	fprintf(f, "Compressed image format 2\n%u %u\n", pixmap->width, pixmap->height);
	
	Word* cWord = pixmap->pixels;

	for (int i = 0; i < (pixmap->width * pixmap->height) /4; i++){
		writeWord(f, *(cWord + i));
	}
}


// writes pnm image to stdout
void writeUncompressed(FILE* f, Pnm_ppm* pixmap){

	// print header
	(void) fprintf(f, "P6\n%d %d\n%d\n", pixmap->width, pixmap->height, pixmap->denominator);
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
		(void)fwrite(color, 1, 3, f); 
	}
	printf("\n");
}

