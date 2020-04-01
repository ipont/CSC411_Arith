#include "operations.h"
#include <stdlib.h> 
#include <math.h>
#include <stdio.h>
#include <stdint.h>

float* DCT(ComponentPix* p1, ComponentPix* p2, ComponentPix* p3, ComponentPix* p4);
void* inverseDCT(Word* in); 

static inline float floatFix(float n, float min, float max){
	if (n > max) return max;
	if (n < min) return min;
	return n;
}

void convertFloat(Pnm_ppm* img){
	RgbPix (*arr) = img->pixels;
	
	for (int i = 0; i < img->height * img->width; i++){
		// this accessing might not work!!!!
		(arr + i)->r /= img->denominator;
		(arr + i)->g /= img->denominator;
		(arr + i)->b /= img->denominator;
	}
}


// takes a Pnm_ppm image, rgb values must already be floating point
void RGBToComponent(Pnm_ppm* img){
	float y, pb, pr;
	RgbPix* pixel = img->pixels;
	ComponentPix* cPixs = (ComponentPix *)malloc(sizeof(ComponentPix)*img->height*img->width);

	for(int i=0; i < img->height*img->width; i++){
		y = 0.299 * pixel->r + 0.587 * pixel->g + 0.114 * pixel->b;
		pb = -0.168736 * pixel->r  - 0.331264 * pixel->g + 0.5 * pixel->b;	
		pr = 0.5 * pixel->r - 0.418688 * pixel->g - 0.081312 * pixel->b;
		
		ComponentPix cPix = {y, pb, pr};
		*(cPixs + i) = cPix;
		pixel += 1;
	}
	free(img->pixels);
	img->pixels = (void *)cPixs;
}

// conver from component to rgb
// img must be floating point
void ComponentToRGB(Pnm_ppm* img){
	float r, g, b;	
	ComponentPix* pixel = img->pixels;
	RgbPix* rgbPixs = (RgbPix *)(malloc(sizeof(RgbPix) * img->height * img->width)); 
	
	for(int i=0; i < img->height * img->width; i++) {
		r = (pixel+i)->y + 1.402 * (pixel+i)->pr;
		g = (pixel+i)->y - (0.344136 * (pixel+1)->pb) - (0.714136 * (pixel+i)->pr);
		b = (pixel+i)->y + (1.772 * (pixel+i)->pb);
		
		r = floatFix(r, 0, 1);
		g = floatFix(g, 0, 1);
		b = floatFix(b, 0, 1);

		*(rgbPixs + i) = (RgbPix){r, g, b};
	}

	free(img->pixels);
	img->pixels = rgbPixs;
}

// img->pixels must be an array of component pixels
void applyDCT(Pnm_ppm* img){
	ComponentPix* cPix = img->pixels;
	int size = (img->width * img->height) / 4;
	Word* words = malloc(sizeof(Word) *size);
	int i = 0;
	for (int r = 0; r < img->height; r += 2){
		for (int c = 0; c < img->width; c += 2){
			
			// p1	p2
			// p3	p4
			ComponentPix* p1 = (cPix + (r * img->width + c));
			ComponentPix* p2 = (cPix + (r * img->width + c + 1));
			ComponentPix* p3 = (cPix + ((r+1) * img->width + c));
			ComponentPix* p4 = (cPix + ((r+1) * img->width + c + 1));
			float* abcd  = DCT( p1, p2, p3, p4);

			float avgPr = (p1->pr + p2->pr + p3->pr + p4->pr) / 4;
			unsigned tPr = Arith_index_of_chroma(avgPr);
			
			float avgPb = (p1->pb + p2->pb + p3->pb + p4->pb) / 4;
			unsigned tPb = Arith_index_of_chroma(avgPb);

			// scale a into unsigned 9 bit representation
			//printf("a: %f\n", *abcd);
			uint64_t a = (uint64_t) (*abcd * 511);
			// truncate floats using round
			int64_t b = round (*(abcd + 1) * 50);
			int64_t c = round (*(abcd + 2) * 50);
			int64_t d = round (*(abcd + 3) * 50);

			*(words + i) = (Word){a, b, c, d, tPb, tPr};
			i++;
			free(abcd);
		}
	}
	free(img->pixels);
	img->pixels = words;
}


float* DCT(ComponentPix* p1, ComponentPix* p2, ComponentPix* p3, ComponentPix* p4){

	float* temp = malloc(sizeof(float) * 4);
	*(temp) = (p4->y + p3->y + p2->y + p1->y) / 4;
	*(temp+1) = (p4->y + p3->y - p2->y - p1->y) / 4;
	*(temp+2) = (p4->y - p3->y + p2->y - p1->y) / 4;
	*(temp+3) = (p4->y - p3->y - p2->y + p1->y) / 4;

	for (int i = 1; i < 4; i++){
		if( *(temp+i) > 0.3)*(temp+i) = (float)0.3;
		else if( *(temp+i) < -0.3)*(temp+i) = (float)-0.3;
	}

	return temp;
}

// img->pixels must be a arrays of word structs
// after function returns img->pixels will be array of ComponentPix structs
void applyIDCT(Pnm_ppm* img){
	Word* cWord = img->pixels;
	ComponentPix* cComp = malloc(sizeof(ComponentPix) * img->width * img->height);

	int i = 0;
	for (int r = 0; r < img->height; r += 2){
		for (int c = 0; c < img->width; c += 2){
		
			float* lums = (float *)inverseDCT((cWord +i));
			// {y, pb, pr}
			// p1	p2
			// p3	p4
			float tPb = Arith_chroma_of_index((cWord+i)->pb);
			float tPr = Arith_chroma_of_index((cWord+i)->pr);
			*(cComp + r * img->width + c) = (ComponentPix){*lums, tPb, tPr}; // p1
			*(cComp + r * img->width + c + 1) = (ComponentPix){*(lums+1), tPb, tPr}; // p2
			*(cComp + (r+1) * img->width + c) = (ComponentPix){*(lums+2), tPb, tPr}; // p3
			*(cComp + (r+1) * img->width + c + 1) = (ComponentPix){*(lums+3), tPb, tPr}; // p4
			free(lums);
			i++;
		}
	}

	free(img->pixels);
	img->pixels = cComp;
}

void* inverseDCT(Word* in){

	float* out = malloc(sizeof(float)*4);
	
	float a = ((float)in->a) / 511; 
	float b = ((float)in->b) / 50;
	float c = ((float)in->c) / 50;
	float d = ((float)in->d) / 50;
	
	//printf("a: %ld,		b: %ld, 	c: %ld, 	d: %ld, 	pb: %d, 	pr: %d\n", in->a, in->b, in->c, in->d, in->pb, in->pr);
	//printf("a: %f,		b: %f, 	c: %f, 	d: %f\n", a, b, c, d);
	
	
	*out = a - b - c + d;
	*(out+1) = a - b + c - d;
	*(out+2) = a + b - c - d;
	*(out+3) = a + b + c + d;

	for (int i = 0; i < 4; i++){
		if (*(out+i) > 1) *(out+i) = 1;
		if (*(out+i) < 0) *(out+i) = 0;
	}
	return out;
}






