
#include "ISense.h"

#ifdef ISENSE_COUNT
#include "ADC.h"
#include "Filter.h"

/*
 * PRIVATE DEFINITIONS
 */

#define CURRENT_FLT_ORDER	7

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static uint16_t ISENSE_GetCurrent(uint32_t ain);

/*
 * PRIVATE VARIABLES
 */

static Filter_t gCurrents[ISENSE_COUNT];

/*
 * PUBLIC FUNCTIONS
 */

void ISENSE_Init(void)
{
	for (uint8_t i = 0; i < ISENSE_COUNT; i++)
	{
		FLT_Init(&gCurrents[i], CURRENT_FLT_ORDER);
	}
}

void ISENSE_Deinit(void)
{
}

void ISENSE_Update(State_t state)
{
	if (state != State_Sleep)
	{
#if ISENSE_COUNT > 0
		FLT_Put(&gCurrents[0], ISENSE_GetCurrent(ISENSE0_AIN));
#endif
#if ISENSE_COUNT > 1
		FLT_Put(&gCurrents[1], ISENSE_GetCurrent(ISENSE1_AIN));
#endif
#if ISENSE_COUNT > 2
		FLT_Put(&gCurrents[2], ISENSE_GetCurrent(ISENSE2_AIN));
#endif
#if ISENSE_COUNT > 3
		FLT_Put(&gCurrents[3], ISENSE_GetCurrent(ISENSE3_AIN));
#endif
	}
}

uint16_t ISENSE_Read(uint8_t ch)
{
	if (ch < ISENSE_COUNT)
	{
		return FLT_Get(&gCurrents[ch]);
	}
	return 0;
}


/*
 * PRIVATE FUNCTIONS
 */

static uint16_t ISENSE_GetCurrent(uint32_t ain)
{
	uint32_t adc = ADC_Read(ain);
	uint32_t microvolts = (adc * ADC_VREF / ISENSE_GAIN) * 1000 / ADC_MAX;
	uint32_t milliamps = microvolts / ISENSE_MOHM;
	return milliamps;
}

/*
 * INTERRUPT ROUTINES
 */

#endif // ISENSE_COUNT
