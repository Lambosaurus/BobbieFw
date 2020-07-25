#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stdbool.h"


//#define BRD_PI
#define BRD_SERVO

#define CORE_USE_HSE

#define LED_GPIO 		GPIOB
#define LED_PIN_RED		GPIO_PIN_1
#define LED_PIN_GRN		GPIO_PIN_0
#define LED_PIN_BLU 	GPIO_PIN_2

#define UART2_GPIO 		GPIOA
#define UART2_PINS		(GPIO_PIN_2 | GPIO_PIN_3)

#define CAN_GPIO		GPIOB
#define CAN_PINS		(GPIO_PIN_8 | GPIO_PIN_9)

#define NTC_AIN  		ADC_CHANNEL_4

#ifdef BRD_PI
// WARN: THIS IS WRONG FOR V1 BOARDS
#define BUTTON_GPIO		GPIOB
#define BUTTON_PIN		GPIO_PIN_3

#define PSU5_EN_GPIO	GPIOA
#define PSU5_EN_PIN		GPIO_PIN_0
#define PSU5_AIN		ADC_CHANNEL_4
#endif //BRD_PI

#ifdef BRD_SERVO

#endif //BRD_SERVO


#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
