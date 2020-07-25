#ifndef GPIO_H
#define GPIO_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

typedef struct GPIO_t {
	GPIO_TypeDef * gpio;
	uint32_t pin;
};

#define GPIO_SET(gpio, pin) 	gpio->BSRR = (uint32_t)pin
#define GPIO_RESET(gpio, pin) 	gpio->BRR = (uint32_t)pin

/*
 * PUBLIC TYPES
 */


#endif //GPIO_H
