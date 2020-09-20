#ifndef FEEDBACK_H
#define FEEDBACK_H

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

void FBK_Init(void);
void FBK_Deinit(void);
void FBK_Update(State_t state);

void FBK_Send(uint8_t addr);

#endif //FEEDBACK_H
