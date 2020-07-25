#ifndef SERVO_H
#define SERVO_H

#include "Board.h"
#include "Module.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

void SERVO_Init(void);
void SERVO_Deinit(void);
void SERVO_Set(uint8_t servo, uint16_t milliseconds);
void SERVO_Update(State_t state);

#endif //SERVO_H
