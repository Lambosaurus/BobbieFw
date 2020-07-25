
#include "ADC.h"
#include "stm32f0xx_hal_adc.h"

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

static ADC_HandleTypeDef gAdc;

/*
 * PUBLIC FUNCTIONS
 */


void ADC_Init(void)
{
	__HAL_RCC_ADC1_CLK_ENABLE();
	gAdc.Instance = ADC1;
	gAdc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	gAdc.Init.Resolution = ADC_RESOLUTION_12B;
	gAdc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	gAdc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
	gAdc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	gAdc.Init.LowPowerAutoWait = DISABLE;
	gAdc.Init.LowPowerAutoPowerOff = DISABLE;
	gAdc.Init.ContinuousConvMode = DISABLE;
	gAdc.Init.DiscontinuousConvMode = DISABLE;
	gAdc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	gAdc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	gAdc.Init.DMAContinuousRequests = DISABLE;
	gAdc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	HAL_ADC_Init(&gAdc);

	HAL_ADCEx_Calibration_Start(&gAdc);
}

uint32_t ADC_Read(uint32_t channel)
{
	// Insert the channel into the regular group
	ADC_ChannelConfTypeDef ch;
	ch.Channel = channel;
	ch.Rank = ADC_RANK_CHANNEL_NUMBER;
	ch.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
	HAL_ADC_ConfigChannel(&gAdc, &ch);

	// Perform the conversion
	HAL_ADC_Start(&gAdc);
	HAL_ADC_PollForConversion(&gAdc, 100);
	uint32_t result = HAL_ADC_GetValue(&gAdc);

	// Remove the channel from the regular group
	ch.Rank = ADC_RANK_NONE;
	HAL_ADC_ConfigChannel(&gAdc, &ch);

	return result;
}

void ADC_Deinit(void)
{
	HAL_ADC_DeInit(&gAdc);
	__HAL_RCC_ADC1_CLK_DISABLE();
}

uint32_t AIN_ToMv(uint32_t ain, uint32_t rlow, uint32_t rhigh)
{
	uint32_t vo = (ain * ADC_VREF) / ADC_MAX;
	return vo * (rhigh + rlow) / rlow;
}

/*
 * PRIVATE FUNCTIONS
 */


/*
 * INTERRUPT ROUTINES
 */
