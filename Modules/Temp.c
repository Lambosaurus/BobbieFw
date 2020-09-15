
#include "Temp.h"
#include "Config.h"
#include "ADC.h"
#include "NTC.h"
#include "Error.h"

/*
 * PRIVATE DEFINITIONS
 */

#define OVERTEMP_HYSTERESIS	5

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

/*
 * PRIVATE VARIABLES
 */

static struct {
	bool overtemp;
	int16_t temp;
} gState;

/*
 * PUBLIC FUNCTIONS
 */

void TEMP_Init(void)
{
	gState.overtemp = false;
}

void TEMP_Update(State_t state)
{
	gState.temp = NTC_10K(ADC_Read( NTC_AIN ));

	int16_t threshold = gCfg.tempLimit;
	if (gState.overtemp) { threshold += OVERTEMP_HYSTERESIS; }
	threshold *= 10;

	gState.overtemp = gState.temp > threshold;
	if (gState.overtemp)
	{
		ERR_Set(ERR_Overtemp);
	}
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */

