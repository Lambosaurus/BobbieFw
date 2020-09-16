
#include "Blink.h"
#include "Config.h"

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static Color_t BLINK_StatusColor(State_t state);

/*
 * PRIVATE VARIABLES
 */

static struct {
	Color_t color;
	uint32_t start;
	uint32_t duration;
	uint8_t blinks;
} gState;

/*
 * PUBLIC FUNCTIONS
 */

void BLINK_Init(void)
{
	LED_Init();
	LED_Set(LED_RED);
	gState.blinks = 0;
}

void BLINK_Deinit(void)
{
	LED_Deinit();
}

void BLINK_Update(State_t state)
{
	Color_t color;
	if (gState.blinks > 0)
	{
		uint32_t now = HAL_GetTick();
		uint32_t elapsed = now - gState.start;
		if (elapsed > gState.duration)
		{
			gState.blinks -= 1;
			color = LED_BLK;
			gState.start = now;
		}
		else
		{
			uint32_t gain = (gState.duration - elapsed) * 256 / gState.duration;
			if (gain > 0xFF) { gain = 0xFF; }
			color = COLOR_Alpha(gState.color, gain);
		}
	}
	else
	{
		color = COLOR_Alpha(BLINK_StatusColor(state), gCfg.ledAlpha);
	}
	LED_Set(color);
}

void BLINK_Start(Color_t color, uint16_t period, uint8_t blinks)
{
	gState.color = color;
	gState.duration = period;
	gState.blinks = blinks;
	gState.start = HAL_GetTick();
}

/*
 * PRIVATE FUNCTIONS
 */

static Color_t BLINK_StatusColor(State_t state)
{
	switch (state)
	{
	case State_Active:
		return LED_GRN;
	case State_Idle:
		return LED_AMB;
	case State_Sleep:
		return LED_BLK;
	case State_Error:
	default:
		return LED_RED;
	}
}

/*
 * INTERRUPT ROUTINES
 */

