#ifndef MOTORS_H
#define MOTORS_H

#include "State.h"
#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

void MOTOR_Init(void);
void MOTOR_Deinit(void);
void MOTOR_Set(uint8_t motor, int16_t throttle);
void MOTOR_Update(State_t state);


#endif //MOTORS_H
