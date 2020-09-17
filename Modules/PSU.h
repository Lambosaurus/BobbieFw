#ifndef PSU_H
#define PSU_H

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

void PSU_Init(void);
void PSU_Deinit(void);
void PSU_Update(State_t state);
uint16_t PSU_GetVoltage(void);

#endif //PSU_H
