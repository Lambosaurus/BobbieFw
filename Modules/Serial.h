#ifndef SERIAL_H
#define SERIAL_H

#include "State.h"
#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

#define SERIAL_SIZE_HEADER	4
#define SERIAL_SIZE_DATA	8
#define SERIAL_SIZE_MAX		(SERIAL_SIZE_HEADER + SERIAL_SIZE_DATA)

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

void SER_Init(void);
void SER_Deinit(void);
void SER_Update(State_t state);

#ifdef SER_USE_BRIDGE
void SER_TxMsg(Msg_t * msg);
#endif

#endif //SERVO_H
