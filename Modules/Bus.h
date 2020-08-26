#ifndef BUS_H
#define BUS_H

#include "State.h"
#include "Board.h"
#include "Messages.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

void BUS_Init(void);
void BUS_Deinit(void);
void BUS_Update(State_t state);

bool BUS_TxMsg(Msg_t * msg);

#ifndef IS_BUSMASTER
void BUS_RecieveState(void);
#endif

#endif //COMMS_H
