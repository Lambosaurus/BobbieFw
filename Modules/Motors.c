
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

/*
 * PRIVATE PROTOTYPES
 */

static void MOTOR_Start(void);
static void MOTOR_Stop(void);
static void MOTOR_Throttle(uint8_t ch, uint8_t i1, uint8_t i2);

/*
 * PRIVATE VARIABLES
 */

static struct {
	bool enabled;
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

void MOTOR_Set(uint8_t motor, int16_t throttle)
{
	if (motor < MOTOR_COUNT)
	{
		bool rev = throttle < 0;
		uint16_t duty = rev ? -throttle : throttle;
		if (duty > MOTOR_PWM_MAX) { duty = MOTOR_PWM_MAX; }

		bool brake = duty < gCfg.motorBrakeThreshold;

		uint8_t i1 = brake ? MOTOR_PWM_MAX 			: duty;
		uint8_t i2 = brake ? MOTOR_PWM_MAX - duty 	: 0   ;

		if (rev)
		{
			MOTOR_Throttle(motor, i2, i1);
		}
		else
		{
			MOTOR_Throttle(motor, i1, i2);
		}
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
}


/*
 * PRIVATE FUNCTIONS
 */

static void MOTOR_Throttle(uint8_t ch, uint8_t i1, uint8_t i2)
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
}

static void MOTOR_Stop(void)
{
	TIM_Stop(MOTOR_TIM);
	HAL_GPIO_DeInit(MOTOR_GPIO, MOTOR_PINS_ALL);
	TIM_Deinit(MOTOR_TIM);
}

/*
 * INTERRUPT ROUTINES
 */

#endif //MOTOR_COUNT
