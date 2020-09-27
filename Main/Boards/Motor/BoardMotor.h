#ifndef BOARD_MOTOR_H
#define BOARD_MOTOR_H

#define BOARD_TYPE 		BoardType_Motor

#define NTC_AIN  		ADC_CHANNEL_4
#define VBAT_AIN		ADC_CHANNEL_5
#define VBAT_RLOW		10
#define VBAT_RHIGH		100

#define USE_MOTORS
#define MOTOR_GPIO		GPIOA
#define MOTOR0_I1_PIN	GPIO_PIN_8
#define MOTOR0_I2_PIN	GPIO_PIN_9
#define MOTOR1_I1_PIN	GPIO_PIN_10
#define MOTOR1_I2_PIN	GPIO_PIN_11
#define MOTOR_GPIO_AF	GPIO_AF2_TIM1
#define MOTOR0_I1_CH	0
#define MOTOR0_I2_CH	1
#define MOTOR1_I1_CH	2
#define MOTOR1_I2_CH	3
#define MOTOR_COUNT		2
#define USE_TIM1
#define MOTOR_TIM		TIM_1
#define MOTOR_FREQ		10000


#define USE_ISENSE
#define ISENSE0_AIN		ADC_CHANNEL_1
#define ISENSE1_AIN		ADC_CHANNEL_0
#define ISENSE_GAIN		20
#define ISENSE_MOHM		25
#define ISENSE_COUNT	2

#define QUAD_GPIO		GPIOB
#define QUAD_2A_PIN		GPIO_PIN_3
#define QUAD_2B_PIN		GPIO_PIN_4
#define QUAD_1A_PIN		GPIO_PIN_5
#define QUAD_1B_PIN		GPIO_PIN_6


#endif // BOARD_MOTOR_H
