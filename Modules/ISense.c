
#include "ISense.h"

#ifdef ISENSE_COUNT
#include "ADC.h"

/*
 * PRIVATE DEFINITIONS
 */

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

uint16_t gCurrents[ISENSE_COUNT];

/*
 * PUBLIC FUNCTIONS
 */

void ISENSE_Init(void)
{
	for (uint8_t i = 0; i < ISENSE_COUNT; i++)
	{
		gCurrents[i] = 0;
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
		gCurrents[0] = ISENSE_GetCurrent(ISENSE0_AIN);
#endif
#if ISENSE_COUNT > 1
		gCurrents[1] = ISENSE_GetCurrent(ISENSE1_AIN);
#endif
#if ISENSE_COUNT > 2
		gCurrents[2] = ISENSE_GetCurrent(ISENSE2_AIN);
#endif
#if ISENSE_COUNT > 3
		gCurrents[3] = ISENSE_GetCurrent(ISENSE3_AIN);
#endif
	}
}

uint16_t ISENSE_Read(uint8_t ch)
{
	if (ch < ISENSE_COUNT)
	{
		return gCurrents[ch];
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
