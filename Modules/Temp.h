
#ifndef TEMP_H
#define TEMP_H

#include "Board.h"
#include "State.h"


void TEMP_Init(void);
void TEMP_Deinit(void);
void TEMP_Update(State_t state);
int16_t TEMP_Get(void);

#endif /* TEMP_H */
