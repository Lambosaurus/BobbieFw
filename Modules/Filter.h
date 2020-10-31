
#ifndef FILTER_H
#define FILTER_H

#include "Board.h"

typedef struct {
	uint32_t reg;
	uint8_t order;
} Filter_t;

// Note, the filter has a time constant of approximately 2^n samples.
// The input value must not exceed 32-n bits.
// Ie, for an order 4 filter:
//     Time constant of 16 samples
//     Input value must not exceed 28 bits.

void FLT_Init(Filter_t * flt, uint8_t order);
void FLT_Clear(Filter_t * flt);
void FLT_Put(Filter_t * flt, uint32_t value);
uint32_t FLT_Get(Filter_t * flt);

#endif /* FILTER_H */
