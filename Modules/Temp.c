
#include "Temp.h"
#include "Config.h"
#include "ADC.h"
#include "NTC.h"
#include "Error.h"

/*
 * PRIVATE DEFINITIONS
 */

#define OVERTEMP_HYSTERESIS		50

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
	gState.temp = 0;
}

void TEMP_Update(State_t state)
{
	gState.temp = NTC_10K(ADC_Read( NTC_AIN ));

	int16_t threshold = gCfg.tempLimit * 10;
	if (gState.overtemp) { threshold += OVERTEMP_HYSTERESIS; }

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

