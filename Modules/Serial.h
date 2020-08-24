#ifndef SERIAL_H
#define SERIAL_H

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

void SER_Init(void);
void SER_Deinit(void);
void SER_Update(State_t state);

#endif //SERVO_H
