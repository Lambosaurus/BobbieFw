
#include "State.h"
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

/*
 * PRIVATE VARIABLES
 */

static struct {
	State_t state;
	uint32_t lastActive;
} gState;

/*
 * PUBLIC FUNCTIONS
 */

State_t State_Update(void)
{
	if (gState.state == State_Active)
	{
		if ( HAL_GetTick() - gState.lastActive > gCfg.activeTimeout)
		{
			gState.state = State_Idle;
		}
	}
	return gState.state;
}

bool State_Req(State_t state)
{
	switch (state)
	{
	case State_Idle:
		if (gState.state != State_Error)
		{
			gState.state = State_Idle;
		}
		break;
	case State_Active:
		gState.lastActive = HAL_GetTick();
		if (gState.state == State_Idle)
		{
			gState.state = State_Active;
		}
		break;
	case State_Error:
		gState.state = State_Error;
		break;
	case State_Sleep:
		if (gState.state == State_Idle)
		{
			gState.state = State_Sleep;
		}
		break;
	}
	return gState.state == state;
}

void State_Reset(void)
{
	gState.state = State_Idle;
}

State_t State_Last(void)
{
	return gState.state;
}


/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */

