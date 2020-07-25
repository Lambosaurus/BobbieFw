#ifndef CAN_H
#define CAN_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef struct {
	uint32_t id;
	uint8_t len;
	uint8_t data[8];
} CANMsg_t;

/*
 * PUBLIC FUNCTIONS
 */

void CAN_Init(void);
void CAN_Deinit(void);
bool CAN_Tx(CANMsg_t * msg);
uint8_t CAN_RxCount(void);
bool CAN_Rx(CANMsg_t * msg);

#endif //CAN_H
