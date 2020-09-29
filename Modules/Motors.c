
#include "Motors.h"

#ifdef MOTOR_COUNT

#include "TIM.h"
#include "GPIO.h"
#include "Config.h"

/*
 * PRIVATE DEFINITIONS
 */

#define MOTOR_PINS_ALL	(MOTOR0_I1_PIN | MOTOR0_I2_PIN | MOTOR1_I1_PIN | MOTOR1_I2_PIN)
#define MOTOR_PWM_MAX	255

/*
 * PRIVATE TYPES
 */

typedef struct {
	int16_t throttle;
	int16_t requested;
} Motor_t;

/*
 * PRIVATE PROTOTYPES
 */

static void MOTOR_Start(void);
static void MOTOR_Stop(void);
static void MOTOR_SetThrottle(uint8_t ch, uint8_t i1, uint8_t i2);
static void MOTOR_UpdateThrottle(uint8_t ch, int16_t throttle);

/*
 * PRIVATE VARIABLES
 */

static struct {
	bool enabled;
	Motor_t motors[MOTOR_COUNT];
} gMotors;

/*
 * PUBLIC FUNCTIONS
 */

void MOTOR_Init(void)
{
	gMotors.enabled = false;
}

void MOTOR_Deinit(void)
{
}

void MOTOR_Set(uint8_t ch, int16_t throttle)
{
	if (ch < MOTOR_COUNT)
	{
		gMotors.motors[ch].requested = throttle;
	}
}

void MOTOR_Update(State_t state)
{
	bool enabled = state == State_Active;

	if (enabled != gMotors.enabled)
	{
		if (enabled)
		{
			MOTOR_Start();
		}
		else
		{
			MOTOR_Stop();
		}
	}

	if (gMotors.enabled)
	{
		for (int i = 0; i < MOTOR_COUNT; i++)
		{
			Motor_t * m = gMotors.motors + i;
			if (m->throttle != m->requested)
			{
				m->throttle = m->requested;
				MOTOR_UpdateThrottle(i, m->throttle);
			}
		}
	}
}


/*
 * PRIVATE FUNCTIONS
 */

static void MOTOR_UpdateThrottle(uint8_t ch, int16_t throttle)
{
	bool rev = throttle < 0;
	uint16_t duty = rev ? -throttle : throttle;
	if (duty > MOTOR_PWM_MAX) { duty = MOTOR_PWM_MAX; }

	bool brake = duty < gCfg.motorBrakeThreshold;

	uint8_t i1 = brake ? MOTOR_PWM_MAX 			: duty;
	uint8_t i2 = brake ? MOTOR_PWM_MAX - duty 	: 0   ;

	if (rev)
	{
		MOTOR_SetThrottle(ch, i2, i1);
	}
	else
	{
		MOTOR_SetThrottle(ch, i1, i2);
	}
}

static void MOTOR_SetThrottle(uint8_t ch, uint8_t i1, uint8_t i2)
{
	switch (ch)
	{
	case 0:
		TIM_SetPulse(MOTOR_TIM, MOTOR0_I1_CH, i1);
		TIM_SetPulse(MOTOR_TIM, MOTOR0_I2_CH, i2);
		break;
	case 1:
		TIM_SetPulse(MOTOR_TIM, MOTOR1_I1_CH, i1);
		TIM_SetPulse(MOTOR_TIM, MOTOR1_I2_CH, i2);
		break;
	}
}

static void MOTOR_Start(void)
{
	TIM_Init(MOTOR_TIM, MOTOR_FREQ * MOTOR_PWM_MAX, MOTOR_PWM_MAX);
	for (int ch = 0; ch < 4; ch++)
	{
		TIM_EnablePwm(MOTOR_TIM, ch);
		TIM_SetPulse(MOTOR_TIM, ch, 0);
	}

	GPIO_InitTypeDef init = {
			.Pin = MOTOR_PINS_ALL,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_HIGH,
			.Mode = GPIO_MODE_AF_PP,
			.Alternate = MOTOR_GPIO_AF,
	};
	HAL_GPIO_Init(MOTOR_GPIO, &init);

	TIM_Start(MOTOR_TIM);

	for (int i = 0; i < MOTOR_COUNT; i++)
	{
		gMotors.motors[i].throttle = 1;
		gMotors.motors[i].requested = 0;
	}

	gMotors.enabled = true;
}

static void MOTOR_Stop(void)
{
	gMotors.enabled = false;

	TIM_Stop(MOTOR_TIM);
	HAL_GPIO_DeInit(MOTOR_GPIO, MOTOR_PINS_ALL);
	TIM_Deinit(MOTOR_TIM);
}

/*
 * INTERRUPT ROUTINES
 */

#endif //MOTOR_COUNT
