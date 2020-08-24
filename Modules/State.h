
#ifndef STATE_H
#define STATE_H

#include "Board.h"

typedef enum {
	State_Active = 0,
	State_Idle = 1,
	State_Sleep = 2,
	State_Error = 3,
} State_t;


void State_Req(State_t state);
State_t State_Update(void);

#endif /* STATE_H */
