#include "operations.h"
#include <stdlib.h> 

// takes a Pnm_ppm image, rgb values must already be floating point
void RGBToComponent(Pnm_ppm* img){
	float y, pb, pr;
	RgbPix* pixel;
	ComponentPix* cPixs = malloc(sizeof(ComponentPix) * img->height * img->width);

	for(int i=0; i < img->height*img->width; i++){
		pixel  = &((RgbPix*)img->pixels)[i];

		y = 0.299 * pixel->r + 0.587 * pixel->g + 0.114 * pixel->b;
		pb = -0.168736 * pixel->r  - 0.331264 * pixel->g + 0.5 * pixel->b;	
		pr = 0.5 * pixel->r - 0.418688 * pixel->g - 0.081312 * pixel->b;
		ComponentPix cPix = {y, pb, pr};
		cPixs[i] = cPix;
	}
}

// conver from component to rgb
// img must be floating point
void ComponentToRGB(Pnm_ppm* img){
	float r, g, b;	
	ComponentPix* pixel;
	RgbPix* rgbPixs = malloc(sizeof(RgbPix) * img->height * img->width); 

	for(int i=0; i < img->height*img->width; i++){
		pixel = &((ComponentPix*)img->pixels)[i];
		r = pixel->y + 1.402 * pixel->pr;
		g = pixel->y - 0.344136 * pixel->pb - 0.714136 * pixel->pr;
		b = pixel->y - 1.772 * pixel->pr;
		
		RgbPix rPix = {r, g, b};
		rgbPixs[i] = rPix;
	}
}

Coeff* DCT(ComponentPix* p1, ComponentPix* p2, ComponentPix* p3, ComponentPix* p4){

	Coeff* out = malloc(sizeof(Coeff));
	float temp[4];
	out->a = (p4->y + p3->y + p2->y + p1->y) / 4;
	temp[1] = (p4->y + p3->y - p2->y - p1->y) / 4;
	temp[2] = (p4->y - p3->y + p2->y - p1->y) / 4;
	temp[3] = (p4->y - p3->y - p2->y + p1->y) / 4;

	for (int i = 1; i < 4; i++){
		if( temp[i] > 0.3)temp[i] = (float)0.3;
		else if( temp[i] < -0.3)temp[i] = (float)-0.3;
	}

	out->b = temp[1];
	out->c = temp[2];
	out->d = temp[3];
	return out;
}

void* inverseDCT(float* a, float* b, float* c, float* d){

	float* out = malloc(sizeof(float)*4);
	out[0] = *a - *b - *c + *d;
	out[1] = *a - *b + *c - *d;
	out[2] = *a + *b - *c - *d;
	out[3] = *a + *b + *c + *d;

	return out;
}






