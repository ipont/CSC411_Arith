#include "compress.h"
#include "pnm.h"

/* The two functions below are functions you should implement.
   They should take their input from the parameter and should
   write their output to stdout */

// reads PPM and writes a compressed image
extern void compress(FILE *input){
	Pnm_ppm img = Pnm_ppmread(stdin, methods);	
}


extern void decompress(FILE *input); // reads a compressed image and writes PPM
