#ifndef STRUCTURES_INCLUDED
#define STRUCTURES_INCLUDED

typedef struct rgbPix {
	float r;
	float g;
	float b;
}RgbPix;

typedef struct componentPix {
	float y;
 	float pr;
	float pb;
}ComponentPix;

typedef struct pnm_ppm {
	int height;
	int width;
	int denominator;
	void* pixels;
}Pnm_ppm;

typedef struct coeff {
	float a;
	float b;
	float c;
	float d;
}Coeff;



#endif
