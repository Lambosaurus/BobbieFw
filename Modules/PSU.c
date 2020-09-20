
#include "PSU.h"

#ifdef USE_PSU
#include "ADC.h"
#include "Error.h"

/*
 * PRIVATE DEFINITIONS
 */

#define PSU_ON		GPIO_PIN_SET
#define PSU_OFF		GPIO_PIN_RESET

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

/*
 * PRIVATE VARIABLES
 */

struct {
	bool isEnabled;
	uint16_t voltage;
	uint32_t startTime;
} gPsu;

/*
 * PUBLIC FUNCTIONS
 */

void PSU_Init()
{
	GPIO_InitTypeDef init = {
			.Pin = PSU_EN_PIN,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_LOW,
			.Mode = GPIO_MODE_OUTPUT_PP
	};
	HAL_GPIO_WritePin(PSU_EN_GPIO, PSU_EN_PIN, PSU_OFF);
	HAL_GPIO_Init(PSU_EN_GPIO, &init);
	gPsu.isEnabled = false;
}

void PSU_Deinit()
{
	HAL_GPIO_DeInit(PSU_EN_GPIO, PSU_EN_PIN);
	gPsu.isEnabled = false;
}

void PSU_Update(State_t state)
{
	bool enabled = state == State_Active;
	if (enabled != gPsu.isEnabled)
	{
		if (enabled)
		{
			gPsu.startTime = HAL_GetTick();
		}
		gPsu.isEnabled = enabled;

		HAL_GPIO_WritePin(PSU_EN_GPIO, PSU_EN_PIN, enabled ? PSU_ON : PSU_OFF);
	}

	gPsu.voltage = AIN_ToMv( ADC_Read(PSU_FB_AIN), PSU_FB_RLOW, PSU_FB_RHIGH);

	if (gPsu.isEnabled && (HAL_GetTick() - gPsu.startTime > PSU_STARTUP_MS))
	{
		if (gPsu.voltage < PSU_MV_MIN)
		{
			ERR_Set(ERR_PsuUndervolt);
		}
	}
	if (gPsu.voltage > PSU_MV_MAX)
	{
		ERR_Set(ERR_PsuOvervolt);
	}
}

uint16_t PSU_GetVoltage()
{
	return gPsu.voltage;
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */

#endif //USE_PSU
