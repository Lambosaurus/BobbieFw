#ifndef COMMS_H
#define COMMS_H

#include "State.h"
#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef enum {
	TOPIC_None = 0,
	TOPIC_BusState = 1,
} Topic_t;

typedef struct {
	uint8_t data[8];
	uint8_t len;
	uint8_t src;
	uint8_t dst;
	uint8_t prio;
	Topic_t topic;
} Msg_t;

/*
 * PUBLIC FUNCTIONS
 */

void COMMS_Init(void);
void COMMS_Deinit(void);
void COMMS_Update(State_t state);

void COMMS_Broadcast(Topic_t topic, uint8_t * data, uint8_t len);
void COMMS_Send(Topic_t topic, uint8_t * data, uint8_t len, uint8_t dest);

#endif //COMMS_H
