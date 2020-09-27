
#include "Motors.h"

#ifdef MOTOR_COUNT

#include "TIM.h"
#include "GPIO.h"

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

void MOTOR_Set(uint8_t motor, int16_t throttle)
{
	if (motor < MOTOR_COUNT)
	{
		bool rev = throttle < 0;
		uint16_t duty = rev ? -throttle : throttle;
		if (duty > MOTOR_PWM_MAX) { duty = MOTOR_PWM_MAX; }
		bool glide = false;
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
	HAL_GPIO_Deinit(MOTOR_GPIO, MOTOR_PINS_ALL);
	TIM_Deinit(MOTOR_TIM);
}

/*
 * INTERRUPT ROUTINES
 */

#endif //MOTOR_COUNT
