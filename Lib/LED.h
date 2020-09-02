#ifndef LED_H
#define LED_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */


#define COLOR_GAIN_MAX	0x0F
#define COLOR(r,g,b) (((r << 12) | (g << 4) | (b >> 4)) & 0x0F0F0F)

typedef enum {
	LED_BLK = COLOR(0,0,0),
	LED_RED = COLOR(0xFF, 0, 0),
	LED_AMB = COLOR(0x90, 0x70, 0),
	LED_YEL = COLOR(0x90, 0x90, 0),
	LED_GRN = COLOR(0,0xFF,0),
	LED_BLU = COLOR(0,0,0xFF),
} Color_t;


/*
 * PUBLIC FUNCTIONS
 */

void LED_Init(void);
void LED_Deinit(void);
void LED_Set(Color_t color);
void LED_Tick(void);

Color_t COLOR_Alpha(Color_t color, uint8_t gain);

#endif //LED_H
