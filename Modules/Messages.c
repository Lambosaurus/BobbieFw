
#include "Bus.h"
#include "Serial.h"
#include "Board.h"
#include "Config.h"
#include <string.h>

/*
 * PRIVATE DEFINITIONS
 */

#define READ_U16(data, i)  ((data[i] << 8) | data[i+1] )
#define READ_U32(data, i)  ((data[i] << 24) | (data[i+1] << 16) | (data[i+2] << 8) | data[i+3])

#define WRITE_U32(data, i, value) do {	\
	data[i  ] = (uint8_t)(value >> 24);	\
	data[i+1] = (uint8_t)(value >> 16);	\
	data[i+2] = (uint8_t)(value >> 8);	\
	data[i+3] = (uint8_t)(value);		\
	} while(0)

#define WRITE_U16(data, i, value) do { \
	data[i  ] = (uint8_t)(value >> 8);	\
	data[i+1] = (uint8_t)(value);		\
	} while(0)


/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void MSG_HandleTopic_Config(Msg_t * msg);
static void MSG_HandleTopic(Msg_t * msg);


/*
 * PUBLIC FUNCTIONS
 */

void MSG_Handle(Msg_t * msg, MsgSrc_t src)
{
	bool matched = msg->dst == gCfg.address;
	if (matched || msg->dst == DST_BROADCAST)
	{
		// Handle message, and do NOT consider propagating to other bus
		MSG_HandleTopic(msg);
	}
#ifdef SER_USE_BRIDGE
	if (src == MsgSrc_Serial)
	{
		// From serial messages get forwarded to bus
		// But not necessary if addressed to the local device
		if (!matched)
		{
			BUS_TxMsg(msg);
		}
	}
	else if (gCfg.serialBridge)
	{
		// From bus messages get forwarded to serial
		SER_TxMsg(msg);
	}
#endif
}


void MSG_Broadcast(Topic_t topic, uint8_t * data, uint8_t len)
{
	MSG_Send(topic, data, len, DST_BROADCAST);
}

void MSG_Send(Topic_t topic, uint8_t * data, uint8_t len, uint8_t dest)
{
	Msg_t msg = {
		.len= len,
		.dst = dest,
		.prio = 1,
		.topic = topic
	};
	memcpy(msg.data, data, len);
	MSG_SendMsg(&msg);
}

void MSG_SendMsg(Msg_t * msg)
{
	msg->src = gCfg.address;
	BUS_TxMsg(msg);

#ifdef SER_USE_BRIDGE
	if (gCfg.serialBridge)
	{
		SER_TxMsg(msg);
	}
#endif
}

/*
 * PRIVATE VARIABLES
 */

static void MSG_HandleTopic(Msg_t * msg)
{
	switch (msg->topic)
	{
#ifndef IS_BUSMASTER
	case TOPIC_BusState:
		BUS_RecieveState();
		break;
#endif
	case TOPIC_Config:
		MSG_HandleTopic_Config(msg);
		break;
	default:
		break;
	}
}


static void MSG_HandleTopic_Config(Msg_t * msg)
{
	uint8_t len = msg->len;
	if (len >= 1)
	{
		switch (msg->data[0])
		{
		case TOPIC_Config_Get:
			if (len >= 3)
			{
				ConfigEnum_t ce = READ_U16(msg->data, 1);
				uint32_t value;
				if (CFG_Get(ce, &value))
				{
					uint8_t data[7];
					data[0] = TOPIC_Config_Is;
					WRITE_U16(data, 1, ce);
					WRITE_U32(data, 3, value);
					MSG_Send(TOPIC_Config, data, sizeof(data), msg->src);
				}
			}
			break;
		case TOPIC_Config_Set:
			if (len >= 7)
			{
				ConfigEnum_t ce = READ_U16(msg->data, 1);
				uint32_t value = READ_U32(msg->data, 3);
				CFG_Set(ce, value);
			}
			break;
		case TOPIC_Config_Load:
			CFG_Load();
			break;
		case TOPIC_Config_Save:
			CFG_Save();
			break;
		case TOPIC_Config_Default:
			CFG_Default();
			break;
		}
	}
}

/*
 * PUBLIC FUNCTIONS
 */

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
