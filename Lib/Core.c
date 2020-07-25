
#include "Core.h"

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void CORE_InitGPIO(void);
static void CORE_InitSysClk(void);

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void CORE_Init(void)
{
	HAL_Init();
	CORE_InitSysClk();
	CORE_InitGPIO();
}

void CORE_Idle(void)
{
	// As long as systick is on, this will at least return each millisecond.
	__WFI();
}

/*
 * PRIVATE FUNCTIONS
 */

void CORE_InitGPIO(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	GPIO_InitTypeDef gpio = {0};
	gpio.Mode = GPIO_MODE_ANALOG;
	gpio.Pull = GPIO_NOPULL;

	// SWCLK and SWDIO on PA13, PA14
	gpio.Pin = GPIO_PIN_All & ~(GPIO_PIN_13 | GPIO_PIN_14);
	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Pin = GPIO_PIN_All;
	HAL_GPIO_Init(GPIOB, &gpio);

	gpio.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOC, &gpio);

	gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_11;
	HAL_GPIO_Init(GPIOF, &gpio);
}

void CORE_InitSysClk(void)
{
	RCC_OscInitTypeDef osc = {0};
#ifdef CORE_USE_HSE
	osc.OscillatorType 		= RCC_OSCILLATORTYPE_HSE;
	osc.HSEState 			= RCC_HSE_ON;
	osc.PLL.PLLState 		= RCC_PLL_ON;
	osc.PLL.PLLSource 		= RCC_PLLSOURCE_HSE;
	osc.PLL.PLLMUL 			= RCC_PLL_MUL2;
	osc.PLL.PREDIV 			= RCC_PREDIV_DIV1;
#else
	osc.OscillatorType 		= RCC_OSCILLATORTYPE_HSI;
	osc.HSIState 			= RCC_HSI_ON;
	osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	osc.PLL.PLLState 		= RCC_PLL_ON;
	osc.PLL.PLLSource 		= RCC_PLLSOURCE_HSI;
	osc.PLL.PLLMUL 			= RCC_PLL_MUL4;
	osc.PLL.PREDIV 			= RCC_PREDIV_DIV1;

#endif //CORE_USE_HSE
	HAL_RCC_OscConfig(&osc);

	RCC_ClkInitTypeDef clk = {0};
	clk.ClockType 		= RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	clk.SYSCLKSource 	= RCC_SYSCLKSOURCE_PLLCLK;
	clk.AHBCLKDivider 	= RCC_SYSCLK_DIV1;
	clk.APB1CLKDivider 	= RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_1);
}


/*
 * CALLBACK FUNCTIONS
 */

void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}

/*
 * INTERRUPT ROUTINES
 */
