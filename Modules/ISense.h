#ifndef ISENSE_H
#define ISENSE_H

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

void ISENSE_Init(void);
void ISENSE_Deinit(void);
void ISENSE_Update(State_t state);
uint16_t ISENSE_Read(uint8_t ch);

#endif //ISENSE_H
