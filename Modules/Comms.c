
#include "Board.h"
#include "Comms.h"
#include "CAN.h"
#include "Error.h"
#include "Config.h"
#include <string.h>

/*
 * PRIVATE DEFINITIONS
 */

#define BUSSTATUS_PERIOD	250
#define BUSSTATUS_TIMEOUT	550

#define DST_BROADCAST		0

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void COMMS_EncodeMsg(Msg_t * msg, CANMsg_t * can);
static void COMMS_DecodeMsg(CANMsg_t * can, Msg_t * msg);
static bool COMMS_RxMsg(Msg_t * msg);
static bool COMMS_TxMsg(Msg_t * msg);


/*
 * PRIVATE VARIABLES
 */

static struct {
	uint32_t lastBusStatus;
} gStatus = { 0 };

/*
 * PUBLIC FUNCTIONS
 */

void COMMS_Init()
{
	CAN_Init();
}

void COMMS_Deinit()
{
	CAN_Deinit();
}

void COMMS_Update(State_t state)
{

#ifdef IS_BUSMASTER
	if (HAL_GetTick() - gStatus.lastBusStatus > BUSSTATUS_PERIOD)
	{
		COMMS_Broadcast(TOPIC_BusState, NULL, 0);
	}
#else
	if (HAL_GetTick() - gStatus.lastBusStatus > BUSSTATUS_PERIOD)
	{
		ERR_Set(ERR_BusTimeout);
	}
#endif
	if (gCfg.address == DST_BROADCAST)
	{
		ERR_Set(ERR_NoAddress);
	}

	Msg_t msg;
	while (COMMS_RxMsg(&msg))
	{
		if (msg.src == gCfg.address)
		{
			ERR_Set(ERR_DuplicateAddress);
		}
		else if (msg.dst == DST_BROADCAST || msg.dst == gCfg.address)
		{
			COMMS_HandleMsg(&msg);
		}
	}
}

void COMMS_Broadcast(Topic_t topic, uint8_t * data, uint8_t len)
{
	COMMS_Send(topic, data, len, DST_BROADCAST);
}

void COMMS_Send(Topic_t topic, uint8_t * data, uint8_t len, uint8_t dest)
{
	Msg_t msg = {
		.len = len,
		.dst = dest,
		.prio = 1,
		.src = gCfg.address,
		.topic = topic
	};
	memcpy(msg.data, data, len);
	COMMS_TxMsg(&msg);
}

void COMMS_SendMsg(Msg_t * msg)
{
	COMMS_TxMsg(msg);
}


void COMMS_HandleMsg(Msg_t * msg)
{
	switch (msg->topic)
	{
#ifndef IS_BUSMASTER
	case TOPIC_BusState:
		gStatus.lastBusStatus = HAL_GetTick();
		break;
#endif
	default:
		break;
	}
}


/*
 * PRIVATE FUNCTIONS
 */

static void COMMS_EncodeMsg(Msg_t * msg, CANMsg_t * can)
{
	memcpy(can->data, msg->data, msg->len);
	can->len = msg->len;
	can->id = (msg->prio << 26)
			| (msg->src << 18)
			| (msg->dst << 10)
			|  msg->topic;
}

static void COMMS_DecodeMsg(CANMsg_t * can, Msg_t * msg)
{
	memcpy(msg->data, can->data, can->len);
	msg->len = can->len;
	uint32_t id = can->id;
	msg->prio 	= (id >> 26) & 0x3;
	msg->src 	= (id >> 18) & 0xFF;
	msg->dst 	= (id >> 10) & 0xFF;
	msg->topic 	= id & 0x3FF;
}

static bool COMMS_RxMsg(Msg_t * msg)
{
	CANMsg_t cmsg;
	if (CAN_Rx(&cmsg))
	{
		COMMS_DecodeMsg(&cmsg, msg);
		return true;
	}
	return false;
}

static bool COMMS_TxMsg(Msg_t * msg)
{
	CANMsg_t cmsg;
	COMMS_EncodeMsg(msg, &cmsg);
	return CAN_Tx(&cmsg);
}


/*
 * INTERRUPT ROUTINES
 */
