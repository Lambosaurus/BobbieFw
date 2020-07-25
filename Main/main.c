/* USER CODE BEGIN Header */

#include "LED.h"
#include "Core.h"
#include "UART.h"
#include "ADC.h"
#include "NTC.h"
#include "Button.h"
#include "CAN.h"

#ifdef USE_PSU
#include "PSU.h"
#endif
#ifdef USE_SERVO
#include "Servo.h"
#endif
#ifdef USE_ISENSE
#include "ISense.h"
#endif

LEDColor_t MAIN_StatusColor(State_t state)
{
	switch (state)
	{
	case State_Active:
		return LED_GRN;
	case State_Idle:
		return LED_AMB;
	case State_Sleep:
		return LED_BLK;
	case State_Error:
	default:
		return LED_RED;
	}
}

int main(void)
{

	CORE_Init();

	LED_Init();
	LED_Set(LED_RED);
	UART_Init(UART_2, 115200);
	CAN_Init();
	ADC_Init();

#ifdef USE_PSU
	PSU_Init();
#endif
#ifdef USE_SERVO
	SERVO_Init();
#endif
#ifdef USE_ISENSE
	ISENSE_Init();
#endif

	while (1)
	{
		uint32_t now = HAL_GetTick();

		State_t state = State_Active;

#ifdef USE_PSU
		PSU_Update(state);
#endif
#ifdef USE_SERVO
		SERVO_Update(state);
#endif
#ifdef USE_ISENSE
		ISENSE_Update(state);
#endif


		/*
		static uint32_t tide = 0;
		if (now - tide > 250)
		{
			uint16_t current = ISENSE_Read(0);
			char bfr[100];
			snprintf( bfr, sizeof(bfr), "I=%d\n", current);
			UART_TxStr(UART_2, bfr);
			tide = now;
		}
		int16_t temp = NTC_10K(ADC_Read( NTC_AIN ));
		*/

		LED_Set( MAIN_StatusColor(state) );

		CORE_Idle();
	}
}

/* USER CODE END Header */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
