#ifndef PSU_H
#define PSU_H

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

void PSU_Init(void);
void PSU_Deinit(void);
void PSU_Update(State_t state);
uint32_t PSU_GetVoltage(void);

#endif //PSU_H
