/* USER CODE BEGIN Header */

#include <Bus.h>
#include "Core.h"
#include "UART.h"
#include "ADC.h"
#include "NTC.h"
#include "Button.h"
#include "Error.h"
#include "Config.h"
#include "Serial.h"
#include "Blink.h"

#ifdef USE_PSU
#include "PSU.h"
#endif
#ifdef USE_SERVO
#include "Servo.h"
#endif
#ifdef USE_ISENSE
#include "ISense.h"
#endif


int main(void)
{

	CORE_Init();
	CFG_Load();

	//CFG_Default();
	//CFG_Save();

	BLINK_Init();
	ADC_Init();

	ERR_Init();
#ifdef USE_PSU
	PSU_Init();
#endif
#ifdef USE_SERVO
	SERVO_Init();
#endif
#ifdef USE_ISENSE
	ISENSE_Init();
#endif
	BUS_Init();
	SER_Init();

	State_Reset();

	while (1)
	{
		State_t state = State_Update();

		BUS_Update(state);
#ifdef USE_PSU
		PSU_Update(state);
#endif
#ifdef USE_SERVO
		SERVO_Update(state);
#endif
#ifdef USE_ISENSE
		ISENSE_Update(state);
#endif
		ERR_Update(state);
		SER_Update(state);

		/*
		uint32_t now = HAL_GetTick();
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

		BLINK_Update(state);

		CORE_Idle();
	}
}

/* USER CODE END Header */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
