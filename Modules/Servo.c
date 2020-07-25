
#include "Servo.h"
#include "TIM.h"
#include "LED.h"
#include "GPIO.h"

/*
 * PRIVATE DEFINITIONS
 */

#define SERVO_PERIOD	3000
#define SERVO_ON		GPIO_PIN_SET
#define SERVO_OFF		GPIO_PIN_RESET
#define SERVO_CHANNELS	4

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void SERVO_Start(void);
static void SERVO_Stop(void);

static void SERVO_StartBatch(uint32_t index);
static void SERVO_ISR_Reload(void);
static void SERVO_ISR_P1(void);
static void SERVO_ISR_P2(void);
static void SERVO_ISR_P3(void);
static void SERVO_ISR_P4(void);

/*
 * PRIVATE VARIABLES
 */

static struct {
	uint8_t index;
	bool running;
}gServoState;

typedef struct {
	GPIO_TypeDef * gpio;
	uint32_t pin;
	uint16_t pulse;
} Servo_t;

Servo_t gServos[SERVO_COUNT] = {
	{ .gpio = S1_GPIO, 	.pin = S1_PIN	},
	{ .gpio = S2_GPIO, 	.pin = S2_PIN	},
	{ .gpio = S3_GPIO, 	.pin = S3_PIN	},
	{ .gpio = S4_GPIO, 	.pin = S4_PIN	},
	{ .gpio = S5_GPIO, 	.pin = S5_PIN	},
	{ .gpio = S6_GPIO, 	.pin = S6_PIN	},
	{ .gpio = S7_GPIO, 	.pin = S7_PIN	},
	{ .gpio = S8_GPIO, 	.pin = S8_PIN	},
	{ .gpio = S9_GPIO, 	.pin = S9_PIN	},
	{ .gpio = S10_GPIO, .pin = S10_PIN	},
	{ .gpio = S11_GPIO, .pin = S11_PIN	},
	{ .gpio = S12_GPIO, .pin = S12_PIN	}
};

/*
 * PUBLIC FUNCTIONS
 */


void SERVO_Init(void)
{
	gServoState.running = false;

	GPIO_InitTypeDef init = {
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_MEDIUM
	};

	for (uint8_t s = 0; s < SERVO_COUNT; s++)
	{
		gServos[s].pulse = 0;
		init.Pin = gServos[s].pin;
		GPIO_RESET(gServos[s].gpio, gServos[s].pin);
		HAL_GPIO_Init(gServos[s].gpio, &init);
	}

	TIM_Init(S_TIMER, 1000000, SERVO_PERIOD);
	TIM_OnReload(S_TIMER, SERVO_ISR_Reload);
	TIM_OnPulse(S_TIMER, 0, SERVO_ISR_P1);
	TIM_OnPulse(S_TIMER, 1, SERVO_ISR_P2);
	TIM_OnPulse(S_TIMER, 2, SERVO_ISR_P3);
	TIM_OnPulse(S_TIMER, 3, SERVO_ISR_P4);
}

void SERVO_Deinit(void)
{
	TIM_Deinit(S_TIMER);

	for (uint8_t s = 0; s < SERVO_COUNT; s++)
	{
		HAL_GPIO_DeInit(gServos[s].gpio, gServos[s].pin);
	}
}

void SERVO_Set(uint8_t servo, uint16_t milliseconds)
{
	if (servo < SERVO_COUNT)
	{
		gServos[servo].pulse = milliseconds;
	}
}

void SERVO_Update(State_t state)
{
	bool running = state == State_Active;

	if (running)
	{
		if (!gServoState.running)
		{
			SERVO_Start();
		}
	}
	else if (gServoState.running)
	{
		SERVO_Stop();
	}
}

/*
 * PRIVATE FUNCTIONS
 */

static void SERVO_Start(void)
{
	gServoState.index = 0;
	gServoState.running = true;
	TIM_Start(S_TIMER);
	SERVO_StartBatch(0);
}

static void SERVO_Stop(void)
{
	gServoState.running = false;
	TIM_Start(S_TIMER);

	for (uint8_t s = 0; s < SERVO_COUNT; s++)
	{
		GPIO_RESET(gServos[s].gpio, gServos[s].pin);
	}
}

static void SERVO_StartBatch(uint32_t index)
{
	uint32_t pulses[SERVO_CHANNELS] = {0};

	for (uint8_t i = 0; i < SERVO_CHANNELS; i++)
	{
		uint8_t s = index + i;
		uint32_t pulse = gServos[s].pulse;
		if (pulse)
		{
			pulses[i] = pulse;
			GPIO_SET(gServos[s].gpio, gServos[s].pin);
		}
	}

	for (uint8_t i = 0; i < SERVO_CHANNELS; i++)
	{
		TIM_SetPulse(S_TIMER, i, pulses[i] ? pulses[i] : SERVO_PERIOD + 1);
	}
}

/*
 * INTERRUPT ROUTINES
 */

static void SERVO_ISR_Reload(void)
{
	gServoState.index = (gServoState.index >= SERVO_COUNT - SERVO_CHANNELS)
						? 0
						: gServoState.index + 4;
	SERVO_StartBatch(gServoState.index);
}

static void SERVO_ISR_P1(void)
{
	uint8_t s = gServoState.index + 0;
	GPIO_RESET(gServos[s].gpio, gServos[s].pin);
}

static void SERVO_ISR_P2(void)
{
	uint8_t s = gServoState.index + 1;
	GPIO_RESET(gServos[s].gpio, gServos[s].pin);
}

static void SERVO_ISR_P3(void)
{
	uint8_t s = gServoState.index + 2;
	GPIO_RESET(gServos[s].gpio, gServos[s].pin);
}

static void SERVO_ISR_P4(void)
{
	uint8_t s = gServoState.index + 3;
	GPIO_RESET(gServos[s].gpio, gServos[s].pin);
}
