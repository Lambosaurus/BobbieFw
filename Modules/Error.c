
#include "Error.h"

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
	}
}

void ERR_Update(State_t state)
{
}

Error_t ERR_Get(void)
{
	return gState.error;
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
