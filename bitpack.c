#include "bitpack.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "except.h"

Except_T Bitpack_Overflow = { "Overflow Packing Bits" };

// =========== helper functions for shifting ============
//
static inline uint64_t leftShift(uint64_t word, unsigned n){
	
	assert(n <= 64);
	if (n == 64)return 0;

	return word << n;
}

static inline uint64_t rightShiftLogical(uint64_t word, unsigned n){
	
	assert(n <= 64);
	if (n == 64)return 0;

	return word >> n;
}

static inline int64_t rightShiftArithmetic(int64_t word, unsigned n){

	assert(n <= 64);
	if (n == 64) return 0;

	uint64_t sign = 1;
	sign  <<= 63;
	sign = sign  & word;

	int64_t out = rightShiftLogical(word, n);
	
	if (sign > 0) {
		out = -1 * out;
		return out;
	}

	return out;
	
}

bool Bitpack_fitsu(uint64_t n, unsigned width){
	
	unsigned bits = 0;
	while (n){
		bits++;
		n >>= 1;
	}
	if (bits <= width)return true;
	return false;
}

bool Bitpack_fitss( int64_t n, unsigned width){
	
	if (n < 0) return (n = ~n);
	
	return Bitpack_fitsu((uint64_t)n, width-1);
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb){
	assert(width <= 64);
	assert(width + lsb <= 64);

	uint64_t out = word;

	out = out << (64-(width+lsb));
	out = out >> (64-width);

	return out; 
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb){
	assert(width <= 64);
	assert(width + lsb <= 64);

	uint64_t shift = leftShift(word, (64-(width+lsb)));
 	
	return rightShiftArithmetic(shift, (64 - width));
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value){

	assert(width <= 64);
	assert(width + lsb <= 64);
	
	if(Bitpack_fitsu(value, width) == false)RAISE(Bitpack_Overflow);

	uint64_t left = rightShiftLogical(word, lsb);
	left = leftShift(left, (64-width));

	uint64_t mid = leftShift(value, lsb);

	uint64_t right = leftShift(word, (64-(width + lsb)));
	right = rightShiftLogical(right, (64-width));

	return left | mid | right;


}
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  int64_t value){

	assert(width <= 64);
	assert(width + lsb <= 64);
	
	if(Bitpack_fitsu(value, width) == false)RAISE(Bitpack_Overflow);

	uint64_t left = rightShiftLogical(word, lsb);
	left = leftShift(left, (64-width));

	uint64_t mid = leftShift(value, lsb);
	uint64_t mask = ~0;
	mask = leftShift(mask, (width-64));
	mask = rightShiftLogical(mask, (64-lsb));
	mid = mask & mid;

	uint64_t right = leftShift(word, (64-(width + lsb)));
	right = rightShiftLogical(right, (64-width));

	return left | mid | right;

}

