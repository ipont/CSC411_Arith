#ifndef OPERATIONS_INCLUDED
#define OPERATIONS_INCLUDED

#include "structures.h" 
#include "arith411.h"

void convertFloat(Pnm_ppm* img);

void RGBToComponent(Pnm_ppm* img);

void ComponentToRGB(Pnm_ppm* img);

void applyDCT(Pnm_ppm* img);

void applyIDCT(Pnm_ppm* img);

#endif
