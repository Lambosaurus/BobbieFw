
#include "Error.h"
#include "Config.h"
#include "Messages.h"

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

struct {
	Error_t error;
	uint32_t lastError;
} gState;

/*
 * PUBLIC FUNCTIONS
 */

void ERR_Init(void)
{
	gState.error = ERR_None;
}

void ERR_Set(Error_t err)
{
	if (gState.error == ERR_None)
	{
		gState.error = err;
		State_Req(State_Error);
		State_Send(DST_BROADCAST);
	}
	gState.lastError = HAL_GetTick();
}

void ERR_Update(State_t state)
{
	if (gState.error != ERR_None && gCfg.errorCooldown != 0)
	{
		if (HAL_GetTick() - gState.lastError > gCfg.errorCooldown)
		{
			ERR_Clear();
		}
	}
}

Error_t ERR_Get(void)
{
	return gState.error;
}

void ERR_Clear(void)
{
	if (gState.error != ERR_None)
	{
		gState.error = ERR_None;
		State_Send(DST_BROADCAST);
	}
	State_Reset();
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
