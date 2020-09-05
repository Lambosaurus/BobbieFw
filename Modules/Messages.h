#ifndef MESSAGES_H
#define MESSAGES_H

#include "State.h"
#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

#define DST_BROADCAST 0


typedef enum {
	BoardType_Pwr,
	BoardType_Pi,
	BoardType_Motor,
	BoardType_Servo
} BoardType_t;

typedef enum {
	TOPIC_None = 0,
	TOPIC_BusState,
	TOPIC_Config,
	TOPIC_State,
} Topic_t;

typedef enum {
	TOPIC_Config_Get,
	TOPIC_Config_Set,
	TOPIC_Config_Default,
	TOPIC_Config_Save,
	TOPIC_Config_Load,
	TOPIC_Config_Is,
} Topic_Config_t;

typedef enum {
	TOPIC_State_Request,
	TOPIC_State_Is,
	TOPIC_State_Blink,
	TOPIC_State_Clear,
} Topic_State_t;

typedef struct {
	uint8_t data[8];
	uint8_t len;
	uint8_t src;
	uint8_t dst;
	uint8_t prio;
	Topic_t topic;
} Msg_t;

typedef enum {
	MsgSrc_Bus,
	MsgSrc_Serial,
} MsgSrc_t;

/*
 * PUBLIC FUNCTIONS
 */

void MSG_Handle(Msg_t * msg, MsgSrc_t src);

void MSG_Broadcast(Topic_t topic, uint8_t * data, uint8_t len);
void MSG_Send(Topic_t topic, uint8_t * data, uint8_t len, uint8_t dest);
void MSG_SendMsg(Msg_t * msg);
void MSG_SendState(uint8_t dest);

#endif //COMMS_H
