
#include "LED.h"

/*
 * PRIVATE DEFINITIONS
 */

#define LED_ON			GPIO_PIN_RESET
#define LED_OFF			GPIO_PIN_SET
#define LED_PIN_ALL		(LED_PIN_RED|LED_PIN_GRN|LED_PIN_BLU)

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
	uint8_t r;
	uint8_t g;
	uint8_t b;
} gColor;

/*
 * PUBLIC FUNCTIONS
 */

void LED_Init(void)
{
	LED_Set(LED_BLK);

	// Turn off LED's
	HAL_GPIO_WritePin(LED_GPIO, LED_PIN_ALL, LED_OFF);

	GPIO_InitTypeDef gpio = {
		.Pin = LED_PIN_ALL,
		.Mode = GPIO_MODE_OUTPUT_OD,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(LED_GPIO, &gpio);
}

void LED_Deinit(void)
{
	HAL_GPIO_DeInit(LED_GPIO, LED_PIN_ALL);
}

Color_t COLOR_Alpha(Color_t color, uint8_t gain)
{
	// gain from 0 to 16
	gain = ((uint32_t)gain + 1) >> 4;
	// times 0-16, div 16
	return ((color * gain) >> 4) & 0x0F0F0F;
}

void LED_Set(Color_t color)
{
	gColor.b = (color      ) & 0xF;
	gColor.g = (color >> 8 ) & 0xF;
	gColor.r = (color >> 16) & 0xF;
}

void LED_Tick(void)
{
	static uint8_t k = 0;
	if (k == 0)
	{
		k = 15;
		HAL_GPIO_WritePin(LED_GPIO, LED_PIN_ALL, LED_OFF);
	}
	k -= 1;
	uint32_t pins = ((gColor.r > k) * LED_PIN_RED)
				  | ((gColor.g > k) * LED_PIN_GRN)
				  | ((gColor.b > k) * LED_PIN_BLU);
	HAL_GPIO_WritePin(LED_GPIO, pins, LED_ON);
}

/*
 * PRIVATE FUNCTIONS
 */


/*
 * INTERRUPT ROUTINES
 */
