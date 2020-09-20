#ifndef BOARD_SERVO_H
#define BOARD_SERVO_H

#define BOARD_TYPE 		BoardType_Servo

#define NTC_AIN  		ADC_CHANNEL_4

#define USE_PSU
#define PSU_EN_GPIO		GPIOC
#define PSU_EN_PIN		GPIO_PIN_13
#define PSU_FB_AIN		ADC_CHANNEL_0
#define PSU_FB_RLOW		10
#define PSU_FB_RHIGH	47
#define PSU_MV_MIN		4000
#define PSU_MV_MAX		8000
#define PSU_STARTUP_MS	200

#define USE_ISENSE
#define ISENSE0_AIN		ADC_CHANNEL_1
#define ISENSE_GAIN		20
#define ISENSE_MOHM		25
#define ISENSE_COUNT	1

#define USE_SERVO
#define USE_TIM3
#define S_TIMER			TIM_3
#define SERVO_COUNT		12
#define S1_GPIO			GPIOA
#define S1_PIN			GPIO_PIN_5
#define S2_GPIO			GPIOA
#define S2_PIN			GPIO_PIN_6
#define S3_GPIO			GPIOA
#define S3_PIN			GPIO_PIN_7
#define S4_GPIO			GPIOB
#define S4_PIN			GPIO_PIN_15
#define S5_GPIO			GPIOA
#define S5_PIN			GPIO_PIN_8
#define S6_GPIO			GPIOA
#define S6_PIN			GPIO_PIN_9
#define S7_GPIO			GPIOA
#define S7_PIN			GPIO_PIN_10
#define S8_GPIO			GPIOA
#define S8_PIN			GPIO_PIN_11
#define S9_GPIO			GPIOB
#define S9_PIN			GPIO_PIN_3
#define S10_GPIO		GPIOB
#define S10_PIN			GPIO_PIN_4
#define S11_GPIO		GPIOB
#define S11_PIN			GPIO_PIN_5
#define S12_GPIO		GPIOB
#define S12_PIN			GPIO_PIN_6


#endif /* BOARD_SERVO_H */
