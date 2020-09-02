
#ifndef BLINK_H
#define BLINK_H

#include "Board.h"
#include "State.h"
#include "LED.h"

void BLINK_Init(void);
void BLINK_Update(State_t state);
void BLINK_Start(Color_t color, uint16_t period, uint8_t blinks);

#endif /* BLINK_H */
