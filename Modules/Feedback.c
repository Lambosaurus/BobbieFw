
#include "Feedback.h"
#include "Board.h"
#include "Config.h"
#include "Messages.h"
#include "Temp.h"

#include "ADC.h"

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

#ifdef VBAT_AIN
	uint16_t battVoltage;
#endif
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

#ifdef VBAT_AIN
		gState.battVoltage = AIN_ToMv(ADC_Read(VBAT_AIN), VBAT_RLOW, VBAT_RHIGH);
#endif

		uint32_t interval = state == State_Active ? gCfg.fbkActiveInterval : gCfg.fbkIdleInterval;
		uint32_t now = HAL_GetTick();
		if (interval > 0 && now - gState.lastFeedbackTime >= interval)
		{
			gState.lastFeedbackTime = now;
			FBK_Send(DST_BROADCAST);
		}
	}
}

#ifdef VBAT_AIN
uint16_t FBK_GetBattVoltage(void)
{
	return gState.battVoltage;
}
#endif

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
#ifdef BRD_PI
	uint16_t pi_v = PSU_GetVoltage();
	uint8_t data[4];
	WRITE_U16(data, 0, temp);
	WRITE_U16(data, 2, pi_v);
	Topic_t topic = Topic_PiFbk;
#endif
#ifdef BRD_MOTOR
	uint16_t motor1_i = ISENSE_Read(0);
	uint16_t motor2_i = ISENSE_Read(1);
	uint16_t batt_v = FBK_GetBattVoltage();
	uint8_t data[8];
	WRITE_U16(data, 0, temp);
	WRITE_U16(data, 2, batt_v);
	WRITE_U16(data, 4, motor1_i);
	WRITE_U16(data, 6, motor2_i);
	Topic_t topic = Topic_MotorFbk;
#endif

	MSG_Send(topic, data, sizeof(data), addr);
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
