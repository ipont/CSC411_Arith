#ifndef STRUCTURES_INCLUDED
#define STRUCTURES_INCLUDED

#include <stdint.h>

typedef struct rgbPix {
	float r;
	float g;
	float b;
}RgbPix;

typedef struct componentPix {
	float y;
 	float pb;
	float pr;
}ComponentPix;

typedef struct pnm_ppm {
	int height;
	int width;
	int denominator;
	void* pixels;
}Pnm_ppm;

typedef struct word {
	uint64_t a;
	int64_t b;
	int64_t c;
	int64_t d;
	uint64_t pb;
	uint64_t pr;
}Word;



#endif
