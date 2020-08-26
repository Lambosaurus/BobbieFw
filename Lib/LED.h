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
	LED_BLK = 0x000000,
	LED_RED = 0x0F0000,
	LED_AMB = 0x090500,
	LED_YEL = 0x090900,
	LED_GRN = 0x000F00,
	LED_BLU = 0x00000F,
} Color_t;

#define COLOR_GAIN_MAX	0x0F

/*
 * PUBLIC FUNCTIONS
 */

void LED_Init(void);
void LED_Deinit(void);
void LED_Set(Color_t color);
void LED_Tick(void);

Color_t COLOR_Alpha(Color_t color, uint8_t gain);

#endif //LED_H
