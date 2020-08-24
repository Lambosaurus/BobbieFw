#ifndef ERROR_H
#define ERROR_H

#include "State.h"
#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef enum {
	ERR_None = 0,
	ERR_BusTimeout,
	ERR_DuplicateAddress,
	ERR_NoAddress,
} Error_t;

/*
 * PUBLIC FUNCTIONS
 */

void ERR_Init(void);
void ERR_Set(Error_t err);
void ERR_Update(State_t state);
Error_t ERR_Get(void);
void ERR_Clear(void);

#endif //CONFIG_H
