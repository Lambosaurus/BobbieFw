/* USER CODE BEGIN Header */

#include <Bus.h>
#include "Core.h"
#include "ADC.h"
#include "Button.h"
#include "Error.h"
#include "Config.h"
#include "Serial.h"
#include "Blink.h"
#include "Temp.h"
#include "Feedback.h"

#ifdef USE_PSU
#include "PSU.h"
#endif
#ifdef USE_SERVO
#include "Servo.h"
#endif
#ifdef USE_ISENSE
#include "ISense.h"
#endif
#ifdef USE_MOTORS
#include "Motors.h"
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
#ifdef USE_MOTORS
	MOTOR_Init();
#endif
	BUS_Init();
	SER_Init();
	TEMP_Init();

	State_Reset();

	while (1)
	{
		State_t state = State_Update();

		TEMP_Update(state);
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
#ifdef USE_MOTORS
		MOTOR_Update(state);
#endif
		ERR_Update(state);
		SER_Update(state);
		FBK_Update(state);

		BLINK_Update(state);

		CORE_Idle();
	}
}

/* USER CODE END Header */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
