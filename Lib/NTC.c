
#include "NTC.h"
#include "ADC.h"

/*
 * PRIVATE DEFINITIONS
 */

#define NTC_T_START 	 1250
#define NTC_T_STEP		-50
#define NTC_STEPS 		 34

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

int32_t TableInterpolate( int32_t * table, int8_t steps, int32_t start, int32_t step, int32_t value);

/*
 * PRIVATE VARIABLES
 */

int32_t gRLookup[NTC_STEPS] = {
	531,
	596,
	672,
	758,
	858,
	974,
	1110,
	1268,
	1452,
	1669,
	1925,
	2228,
	2586,
	3014,
	3535,
	4161,
	4917,
	5834,
	6948,
	8315,
	10000,
	12081,
	14674,
	17926,
	22021,
	27219,
	33892,
	42506,
	53650,
	68237,
	87559,
	113347,
	148171,
	195652,
};



/*
 * PUBLIC FUNCTIONS
 */

int16_t NTC_10K(uint32_t adc)
{
	uint32_t r = 10000 * adc;
	if (adc < ADC_MAX)
	{
		r /= (ADC_MAX - adc);
	}
	return (int16_t)TableInterpolate(gRLookup, NTC_STEPS, NTC_T_START, NTC_T_STEP, r);
}

/*
 * PRIVATE FUNCTIONS
 */

int32_t TableInterpolate( int32_t * table, int8_t steps, int32_t start, int32_t step, int32_t value)
{
	if (value < table[0])
	{
		return start;
	}
	else if (value > table[steps-1])
	{
		return start + (step * steps);
	}

	int8_t i = 1;
	while ( value > table[i] ) { i++; }
	uint32_t p0 = table[i-1];
	uint32_t p1 = table[i];

	int32_t alpha = (value - p0) * 1000 / (p1 - p0);
	int32_t result = start + (i * step) + (alpha * step / 1000);
	return result;
}

/*
 * INTERRUPT ROUTINES
 */
