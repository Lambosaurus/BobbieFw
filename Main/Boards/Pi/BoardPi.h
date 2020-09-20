#ifndef BOARD_PI_H
#define BOARD_PI_H

#define BOARD_TYPE 		BoardType_Pi

#define NTC_AIN  		ADC_CHANNEL_1

// WARN: THIS IS WRONG FOR V1 BOARDS
#define BUTTON_GPIO		GPIOB
#define BUTTON_PIN		GPIO_PIN_3

#define PSU5_EN_GPIO	GPIOA
#define PSU5_EN_PIN		GPIO_PIN_0
#define PSU5_AIN		ADC_CHANNEL_4


#endif /* BOARD_PI_H */
