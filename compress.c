#include "compress.h"
#include "pnmrdr.h"
#include "structures.h"
#include "operations.h"
#include "imageIO.h"
#include <stdio.h>
#include "bitpack.h"

/* The two functions below are functions you should implement.
   They should take their input from the parameter and should
   write their output to stdout */

// reads PPM and writes a compressed image
void compress(FILE *input){
	Pnm_ppm* img = readUncompressed(input);
	convertFloat(img);
	RGBToComponent(img);
	applyDCT(img);
	applyIDCT(img);
	ComponentToRGB(img);
	//RgbPix* cPix = img->pixels;
	//for (int i = 0; i < img->height * img->width; i++){
	//	int r = (cPix+i)->r * img->denominator;
	//	int g = (cPix+i)->g * img->denominator;
	//	int b = (cPix+i)->b * img->denominator;
	//	printf("%d  %d  %d\n", r, g, b);
	//}
	writeUncompressed(input, img);	
	//
	return;
}
// reads a compressed image and writes PPM
void decompress(FILE *input){
	Pnm_ppm* img = readCompressed(input);
	applyIDCT(img);
	ComponentToRGB(img);
	writeUncompressed(input, img);
	return;
}
