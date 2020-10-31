
#include "Filter.h"


/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void FLT_Init(Filter_t * flt, uint8_t order)
{
	flt->order = order;
	FLT_Clear(flt);
}

void FLT_Clear(Filter_t * flt)
{
	flt->reg = 0;
}

void FLT_Put(Filter_t * flt, uint32_t value)
{
	flt->reg += value - (flt->reg >> flt->order);
}

uint32_t FLT_Get(Filter_t * flt)
{
	return flt->reg >> flt->order;
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */

