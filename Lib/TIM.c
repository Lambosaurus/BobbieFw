
#include "TIM.h"

/*
 * PRIVATE DEFINITIONS
 */

#define TIM_ENABLE_CCx(tim, cc)   	(tim->Instance->CCER |=  (TIM_CCER_CC1E << (cc*4)))
#define TIM_DISABLE_CCx(tim, cc) 	(tim->Instance->CCER &= ~(TIM_CCER_CC1E << (cc*4)))

#define TIM_CCMRx_MSK	(TIM_CCMR1_OC1M | TIM_CCMR1_CC1S | TIM_CCMR1_OC1FE | TIM_CCMR1_OC1PE)


/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void TIMx_Init(TIM_t * tim);
static void TIMx_Deinit(TIM_t * tim);
static void TIM_IRQHandler(TIM_t * tim);
static void TIM_Reload(TIM_t * tim);

static void TIM_EnableOCx(TIM_t * tim, uint8_t oc, uint32_t mode);

/*
 * PRIVATE VARIABLES
 */

#ifdef USE_TIM1
static TIM_t gTIM_1 = {
	.Instance = TIM1
};
TIM_t * TIM_1 = &gTIM_1;
#endif
#ifdef USE_TIM2
static TIM_t gTIM_2 = {
	.Instance = TIM2
};
TIM_t * TIM_2 = &gTIM_2;
#endif
#ifdef USE_TIM3
static TIM_t gTIM_3 = {
	.Instance = TIM3
};
TIM_t * TIM_3 = &gTIM_3;
#endif
#ifdef USE_TIM14
static TIM_t gTIM_14 = {
	.Instance = TIM14
};
TIM_t * TIM_14 = &gTIM_14;
#endif
#ifdef USE_TIM16
static TIM_t gTIM_16 = {
	.Instance = TIM16
};
TIM_t * TIM_16 = &gTIM_16;
#endif
#ifdef USE_TIM17
static TIM_t gTIM_17 = {
	.Instance = TIM17
};
TIM_t * TIM_17 = &gTIM_17;
#endif

/*
 * PUBLIC FUNCTIONS
 */

void TIM_Init(TIM_t * tim, uint32_t freq, uint16_t reload)
{
	TIMx_Init(tim);
	uint32_t sysclk = HAL_RCC_GetPCLK1Freq();

	uint32_t cr1 = tim->Instance->CR1;
	cr1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS | TIM_CR1_CKD | TIM_CR1_ARPE);
	cr1 |= TIM_AUTORELOAD_PRELOAD_ENABLE | TIM_CLOCKDIVISION_DIV1 | TIM_COUNTERMODE_UP;
	tim->Instance->CR1 = cr1;

	/* Set the Autoreload value */
	tim->Instance->ARR = (uint32_t)reload;

	/* Set the Prescaler value */
	tim->Instance->PSC = (sysclk / freq) - 1;
}

void TIM_OnReload(TIM_t * tim, VoidFunction_t callback)
{
	__HAL_TIM_ENABLE_IT(tim, TIM_IT_UPDATE);
	tim->ReloadCallback = callback;
}

void TIM_OnPulse(TIM_t * tim, uint8_t ch, VoidFunction_t callback)
{
	if (ch < 4)
	{
		TIM_EnableOCx(tim, ch, TIM_OCMODE_ACTIVE);
		// Note that the channels IT's are 1 << 1 through 1 << 4
		__HAL_TIM_ENABLE_IT(tim, TIM_IT_CC1 << ch);
		tim->PulseCallback[ch] = callback;
	}
}

void TIM_EnablePwm(TIM_t * tim, uint8_t ch)
{
	if (ch < 4)
	{
		if(IS_TIM_BREAK_INSTANCE(tim->Instance) != RESET)
		{
			__HAL_TIM_MOE_ENABLE(tim);
		}

		// TIM_CCMR1_OC1PE is the output compare preload
		TIM_EnableOCx(tim, ch, TIM_OCMODE_PWM1 | TIM_CCMR1_OC1PE | TIM_OCFAST_ENABLE);
	}
}


void TIM_SetPulse(TIM_t * tim, uint8_t ch, uint16_t pulse)
{
	switch (ch)
	{
	case 0:
		tim->Instance->CCR1 = pulse;
		break;
	case 1:
		tim->Instance->CCR2 = pulse;
		break;
	case 2:
		tim->Instance->CCR3 = pulse;
		break;
	case 3:
		tim->Instance->CCR4 = pulse;
		break;
	}

}

void TIM_Start(TIM_t * tim)
{
	TIM_Reload(tim);
	__HAL_TIM_ENABLE(tim);
}

void TIM_Stop(TIM_t * tim)
{
	__HAL_TIM_DISABLE(tim);
}

void TIM_Deinit(TIM_t * tim)
{
	__HAL_TIM_DISABLE(tim);
	__HAL_TIM_DISABLE_IT(tim, TIM_IT_UPDATE | TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4);
	TIMx_Deinit(tim);
}

/*
 * PRIVATE FUNCTIONS
 */

static void TIM_EnableOCx(TIM_t * tim, uint8_t oc, uint32_t mode)
{
	// Disable the channel during the update.
	TIM_DISABLE_CCx(tim, oc);
	switch (oc)
	{
	case 0:
		MODIFY_REG(tim->Instance->CCMR1, TIM_CCMRx_MSK, mode);
		MODIFY_REG(tim->Instance->CCER, TIM_CCER_CC1P, TIM_OCPOLARITY_HIGH);
		break;
	case 1:
		MODIFY_REG(tim->Instance->CCMR1, TIM_CCMRx_MSK << 8, mode << 8);
		MODIFY_REG(tim->Instance->CCER, TIM_CCER_CC2P, TIM_OCPOLARITY_HIGH << 4);
		break;
	case 2:
		MODIFY_REG(tim->Instance->CCMR2, TIM_CCMRx_MSK, mode);
		MODIFY_REG(tim->Instance->CCER, TIM_CCER_CC3P, TIM_OCPOLARITY_HIGH << 8);
		break;
	case 3:
		MODIFY_REG(tim->Instance->CCMR2, TIM_CCMRx_MSK << 8, mode << 8);
		MODIFY_REG(tim->Instance->CCER, TIM_CCER_CC4P, TIM_OCPOLARITY_HIGH << 12);
		break;
	}
	TIM_ENABLE_CCx(tim, oc);

	//if(IS_TIM_CCXN_INSTANCE(tim->Instance, TIM_CHANNEL_1))
	//{
	//	MODIFY_REG(tmpccer, TIM_CCER_CC1NP, TIM_OCNPOLARITY_LOW);
	//	tmpccer &= ~TIM_CCER_CC1NE;
	//}

	//if(IS_TIM_BREAK_INSTANCE(TIMx))
	//{
	//	uint32_t tmpcr2 =  TIMx->CR2;
	//	MODIFY_REG(tmpcr2, TIM_CR2_OIS1 | TIM_CR2_OIS1N, TIM_OCIDLESTATE_SET | TIM_OCNIDLESTATE_SET);
	//	TIMx->CR2 = tmpcr2;
	//}


}

static void TIM_Reload(TIM_t * tim)
{
	// Disable all timer event sources.
	uint32_t itSources = tim->Instance->DIER;
	tim->Instance->DIER = 0;

	// Update the prescalar
	tim->Instance->EGR = TIM_EGR_UG;

	// Clear the event before it occurrs.
	__HAL_TIM_CLEAR_IT(tim, TIM_IT_UPDATE);
	tim->Instance->DIER = itSources;
}

static void TIMx_Init(TIM_t * tim)
{
#ifdef USE_TIM1
	if (tim == TIM_1)
	{
		HAL_NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
		HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
		__HAL_RCC_TIM1_CLK_ENABLE();
	}

#endif
#ifdef USE_TIM2
	if (tim == TIM_2)
	{
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
		__HAL_RCC_TIM2_CLK_ENABLE();
	}
#endif
#ifdef USE_TIM3
	if (tim == TIM_3)
	{
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
		__HAL_RCC_TIM3_CLK_ENABLE();
	}
#endif
#ifdef USE_TIM14
	if (tim == TIM_14)
	{
		HAL_NVIC_EnableIRQ(TIM14_IRQn);
		__HAL_RCC_TIM14_CLK_ENABLE();
	}
#endif
#ifdef USE_TIM16
	if (tim == TIM_16)
	{
		HAL_NVIC_EnableIRQ(TIM16_IRQn);
		__HAL_RCC_TIM16_CLK_ENABLE();
	}
#endif
#ifdef USE_TIM17
	if (tim == TIM_17)
	{
		HAL_NVIC_EnableIRQ(TIM17_IRQn);
		__HAL_RCC_TIM17_CLK_ENABLE();
	}
#endif
}


static void TIMx_Deinit(TIM_t * tim)
{
#ifdef USE_TIM1
	if (tim == TIM_1)
	{
		HAL_NVIC_DisableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
		HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
		__HAL_RCC_TIM1_CLK_DISABLE();
	}

#endif
#ifdef USE_TIM2
	if (tim == TIM_2)
	{
		HAL_NVIC_DisableIRQ(TIM2_IRQn);
		__HAL_RCC_TIM2_CLK_DISABLE();
	}
#endif
#ifdef USE_TIM3
	if (tim == TIM_3)
	{
		HAL_NVIC_DisableIRQ(TIM3_IRQn);
		__HAL_RCC_TIM3_CLK_DISABLE();
	}
#endif
#ifdef USE_TIM14
	if (tim == TIM_14)
	{
		HAL_NVIC_DisableIRQ(TIM14_IRQn);
		__HAL_RCC_TIM14_CLK_DISABLE();
	}
#endif
#ifdef USE_TIM16
	if (tim == TIM_16)
	{
		HAL_NVIC_DisableIRQ(TIM16_IRQn);
		__HAL_RCC_TIM16_CLK_DISABLE();
	}
#endif
#ifdef USE_TIM17
	if (tim == TIM_17)
	{
		HAL_NVIC_DisableIRQ(TIM17_IRQn);
		__HAL_RCC_TIM17_CLK_DISABLE();
	}
#endif
}

/*
 * INTERRUPT ROUTINES
 */

static void TIM_IRQHandler(TIM_t * tim)
{
	if(__HAL_TIM_GET_FLAG(tim, TIM_FLAG_CC1) != RESET)
	{
		__HAL_TIM_CLEAR_IT(tim, TIM_IT_CC1);
		tim->PulseCallback[0]();
	}
	if(__HAL_TIM_GET_FLAG(tim, TIM_FLAG_CC2) != RESET)
	{
		__HAL_TIM_CLEAR_IT(tim, TIM_IT_CC2);
		tim->PulseCallback[1]();
	}
	if(__HAL_TIM_GET_FLAG(tim, TIM_FLAG_CC3) != RESET)
	{
		__HAL_TIM_CLEAR_IT(tim, TIM_IT_CC3);
		tim->PulseCallback[2]();
	}
	if(__HAL_TIM_GET_FLAG(tim, TIM_FLAG_CC4) != RESET)
	{
		__HAL_TIM_CLEAR_IT(tim, TIM_IT_CC4);
		tim->PulseCallback[3]();
	}
	if(__HAL_TIM_GET_FLAG(tim, TIM_FLAG_UPDATE) != RESET)
	{
		__HAL_TIM_CLEAR_IT(tim, TIM_IT_UPDATE);
		tim->ReloadCallback();
	}
}

#ifdef USE_TIM1
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	TIM_IRQHandler(TIM_1);
}
void TIM1_CC_IRQHandler(void)
{
	TIM_IRQHandler(TIM_1);
}
#endif
#ifdef USE_TIM2
void TIM2_IRQHandler(void)
{
	TIM_IRQHandler(TIM_2);
}
#endif
#ifdef USE_TIM3
void TIM3_IRQHandler(void)
{
	TIM_IRQHandler(TIM_3);
}
#endif
#ifdef USE_TIM14
void TIM14_IRQHandler(void)
{
	TIM_IRQHandler(TIM_14);
}
#endif
#ifdef USE_TIM16
void TIM16_IRQHandler(void)
{
	TIM_IRQHandler(TIM_16);
}
#endif
#ifdef USE_TIM17
void TIM17_IRQHandler(void)
{
	TIM_IRQHandler(TIM_17);
}
#endif

