#ifndef BOARD_PI_H
#define BOARD_PI_H

#define BOARD_TYPE 		BoardType_Pi

#define NTC_AIN  		ADC_CHANNEL_1

// WARN: THIS IS WRONG FOR V1 BOARDS
#define BUTTON_GPIO		GPIOB
#define BUTTON_PIN		GPIO_PIN_3

#define USE_PSU
#define PSU_EN_GPIO		GPIOA
#define PSU_EN_PIN		GPIO_PIN_0
#define PSU_FB_AIN		ADC_CHANNEL_4
#define PSU_FB_RLOW		10
#define PSU_FB_RHIGH	47
#define PSU_MV_MIN		4500
#define PSU_MV_MAX		5500
#define PSU_STARTUP_MS	200


#endif /* BOARD_PI_H */
