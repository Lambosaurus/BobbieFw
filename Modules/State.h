
#ifndef STATE_H
#define STATE_H

#include "Board.h"

typedef enum {
	State_Active = 0,
	State_Idle = 1,
	State_Sleep = 2,
	State_Error = 3,
} State_t;


bool State_Req(State_t state);
State_t State_Update(void);
void State_Reset(void);
State_t State_Last(void);

#endif /* STATE_H */
