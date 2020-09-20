
#include "Feedback.h"
#include "Board.h"
#include "Config.h"
#include "Messages.h"
#include "Temp.h"

#ifdef USE_ISENSE
#include "Isense.h"
#endif
#ifdef USE_PSU
#include "PSU.h"
#endif

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
		uint32_t interval = state == State_Active ? gCfg.fbkActiveInterval : gCfg.fbkIdleInterval;
		uint32_t now = HAL_GetTick();
		if (interval > 0 && now - gState.lastFeedbackTime >= interval)
		{
			gState.lastFeedbackTime = now;
			FBK_Send(DST_BROADCAST);
		}
	}
}

void FBK_Send(uint8_t addr)
{
	int16_t temp = TEMP_Get();

#ifdef BRD_SERVO
	uint16_t servo_i = ISENSE_Read(0);
	uint16_t servo_v = PSU_GetVoltage();
	uint8_t data[6];
	WRITE_U16(data, 0, temp);
	WRITE_U16(data, 2, servo_i);
	WRITE_U16(data, 4, servo_v);
	Topic_t topic = Topic_ServoFbk;
#endif

	MSG_Send(topic, data, sizeof(data), addr);
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
