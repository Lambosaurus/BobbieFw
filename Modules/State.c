
#include "State.h"

/*
 * PRIVATE DEFINITIONS
 */

#define ACTIVE_TIMEOUT	1000

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
} gState = {
	.state = State_Idle,
	.lastActive = 0,
};

/*
 * PUBLIC FUNCTIONS
 */

State_t State_Update(void)
{
	if (gState.state == State_Active)
	{
		if ( HAL_GetTick() - gState.lastActive > ACTIVE_TIMEOUT)
		{
			gState.state = State_Idle;
		}
	}
	return gState.state;
}

void State_Req(State_t state)
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
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */

