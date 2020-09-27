
#include "Motors.h"

#ifdef MOTOR_COUNT

#include "TIM.h"
#include "GPIO.h"

/*
 * PRIVATE DEFINITIONS
 */

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

typedef struct {
	int16_t throttle;
} Motor_t;

static struct {
	bool enabled;
	Motor_t motors[MOTOR_COUNT];
} gMotors;

/*
 * PUBLIC FUNCTIONS
 */

void MOTOR_Init(void)
{
	GPIO_InitTypeDef init = {
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_HIGH,
	};
	init.Pin = MOTOR0_I1_PIN | MOTOR0_I2_PIN;
	HAL_GPIO_Init(MOTOR_GPIO, &init);
	init.Pin = MOTOR1_I1_PIN | MOTOR1_I2_PIN;
	HAL_GPIO_Init(MOTOR_GPIO, &init);

	gMotors.enabled = false;
}

void MOTOR_Deinit(void)
{
	HAL_GPIO_DeInit(MOTOR_GPIO, MOTOR0_I1_PIN | MOTOR0_I2_PIN);
	HAL_GPIO_DeInit(MOTOR_GPIO, MOTOR1_I1_PIN | MOTOR1_I2_PIN);
}

void MOTOR_Set(uint8_t motor, int16_t throttle)
{
	if (motor < MOTOR_COUNT)
	{
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
	else
	{
		static bool once = true;
		if (once)
		{
			once = false;
			GPIO_RESET(MOTOR_GPIO, MOTOR0_I1_PIN);

			GPIO_InitTypeDef init = {
					.Pin = MOTOR0_I2_PIN,
					.Pull = GPIO_NOPULL,
					.Speed = GPIO_SPEED_FREQ_HIGH,
					.Mode = GPIO_MODE_AF_PP,
					.Alternate = GPIO_AF2_TIM1,
			};
			HAL_GPIO_Init(MOTOR_GPIO, &init);

			TIM_Init(TIM_1, 5000 * 255,  255);
			TIM_EnablePwm(TIM_1, 1);
			TIM_SetPulse(TIM_1, 1, 0);
			TIM_Start(TIM_1);
		}
	}
}


/*
 * PRIVATE FUNCTIONS
 */

static void MOTOR_Start(void)
{
}

static void MOTOR_Stop(void)
{
}

/*
 * INTERRUPT ROUTINES
 */

#endif //MOTOR_COUNT
