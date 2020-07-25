#ifndef LED_H
#define LED_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef enum {
	LED_BLK = 0x000,
	LED_RED = 0x800,
	LED_AMB = 0x530,
	LED_YEL = 0x550,
	LED_GRN = 0x080,
	LED_BLU = 0x008,
} LEDColor_t;

/*
 * PUBLIC FUNCTIONS
 */

void LED_Init(void);
void LED_Deinit(void);
void LED_Set(LEDColor_t color);
void LED_Tick(void);

#endif //LED_H
