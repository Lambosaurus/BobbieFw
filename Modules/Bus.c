
#include "Bus.h"
#include "Board.h"
#include "CAN.h"
#include "Error.h"
#include "Config.h"
#include <string.h>

/*
 * PRIVATE DEFINITIONS
 */

#define BUSSTATUS_PERIOD	250
#define BUSSTATUS_TIMEOUT	550


/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void BUS_EncodeMsg(Msg_t * msg, CANMsg_t * can);
static void BUS_DecodeMsg(CANMsg_t * can, Msg_t * msg);
static bool BUS_RxMsg(Msg_t * msg);


/*
 * PRIVATE VARIABLES
 */

static struct {
	uint32_t lastBusStatus;
} gStatus = { 0 };

/*
 * PUBLIC FUNCTIONS
 */

void BUS_Init()
{
	CAN_Init();
}

void BUS_Deinit()
{
	CAN_Deinit();
}

void BUS_Update(State_t state)
{

#ifdef IS_BUSMASTER
	if (HAL_GetTick() - gStatus.lastBusStatus > BUSSTATUS_PERIOD)
	{
		MSG_Broadcast(TOPIC_BusState, NULL, 0);
		gStatus.lastBusStatus = HAL_GetTick();
	}
#else
	if (HAL_GetTick() - gStatus.lastBusStatus > BUSSTATUS_TIMEOUT)
	{
		ERR_Set(ERR_BusTimeout);
	}
#endif
	if (gCfg.address == DST_BROADCAST)
	{
		ERR_Set(ERR_NoAddress);
	}

	Msg_t msg;
	while (BUS_RxMsg(&msg))
	{
		if (msg.src == gCfg.address)
		{
			ERR_Set(ERR_DuplicateAddress);
		}
		else if (msg.dst == DST_BROADCAST || msg.dst == gCfg.address)
		{
			MSG_Handle(&msg, MsgSrc_Bus);
		}
	}
}

#ifndef IS_BUSMASTER
void BUS_RecieveState(void)
{
	gStatus.lastBusStatus = HAL_GetTick();
}
#endif

/*
 * PRIVATE FUNCTIONS
 */

static void BUS_EncodeMsg(Msg_t * msg, CANMsg_t * can)
{
	memcpy(can->data, msg->data, msg->len);
	can->len = msg->len;
	can->id = (msg->prio << 26)
			| (msg->src << 18)
			| (msg->dst << 10)
			|  msg->topic;
}

static void BUS_DecodeMsg(CANMsg_t * can, Msg_t * msg)
{
	memcpy(msg->data, can->data, can->len);
	msg->len = can->len;
	uint32_t id = can->id;
	msg->prio 	= (id >> 26) & 0x3;
	msg->src 	= (id >> 18) & 0xFF;
	msg->dst 	= (id >> 10) & 0xFF;
	msg->topic 	= id & 0x3FF;
}

static bool BUS_RxMsg(Msg_t * msg)
{
	CANMsg_t cmsg;
	if (CAN_Rx(&cmsg))
	{
		BUS_DecodeMsg(&cmsg, msg);
		return true;
	}
	return false;
}

bool BUS_TxMsg(Msg_t * msg)
{
	CANMsg_t cmsg;
	BUS_EncodeMsg(msg, &cmsg);
	return CAN_Tx(&cmsg);
}


/*
 * INTERRUPT ROUTINES
 */
