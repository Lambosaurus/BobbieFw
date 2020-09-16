
#include "Feedback.h"
#include "Board.h"
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
	uint32_t lastFeedbackTime;
} gState;

/*
 * PUBLIC FUNCTIONS
 */

void FBK_Init(void)
{
	gState.lastFeedbackTime = HAL_GetTick();
}

void FBK_Deinit(void)
{
}

void FBK_Update(State_t state)
{
	if (state != State_Sleep)
	{

	}
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
