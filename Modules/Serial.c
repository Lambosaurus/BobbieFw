
#include "Serial.h"
#include "UART.h"
#include "Comms.h"

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

typedef enum {
	DECODE_START,
	DECODE_HEADER,
	DECODE_TOPIC,
	DECODE_DST,
	DECODE_DATA,
} DecodeState_t;

#define START_CHAR 	0x5F

#define SERIAL_TIMEOUT_MS	100

#define HEADER_MASK_LEN			0x0F
#define HEADER_MASK_FLAGS		0x30
#define HEADER_MASK_TOPIC		0xC0

#define FLAG_TO_LOCAL			0x10
#define FLAG_TO_BUS				0x20

/*
 * PRIVATE PROTOTYPES
 */

void SER_Read(void);
void SER_HandleChar(char ch);
void SER_HandleMsg(Msg_t * msg, uint8_t flags);

/*
 * PRIVATE VARIABLES
 */

static struct {
	struct {
		uint16_t index;
		DecodeState_t state;
		Msg_t msg;
		uint8_t flags;
		uint32_t timeout;
	}rx;
} gState;

/*
 * PUBLIC FUNCTIONS
 */

void SER_Init(void)
{
	UART_Init(SER_UART, 115200);
	gState.rx.state = DECODE_START;
}

void SER_Deinit(void)
{
	UART_Deinit(SER_UART);
}

void SER_Update(State_t state)
{
	SER_Read();
}

/*
 * PRIVATE FUNCTIONS
 */

void SER_Read(void)
{
	uint16_t ready = UART_RxCount(SER_UART);
	if (ready)
	{
		gState.rx.timeout = HAL_GetTick();
		while (ready--)
		{
			char ch = UART_RxPop(SER_UART);
			SER_HandleChar(ch);
		}
	}
	else if (gState.rx.state != DECODE_START)
	{
		if (HAL_GetTick() - gState.rx.timeout > SERIAL_TIMEOUT_MS)
		{
			gState.rx.state = DECODE_START;
		}
	}
}

void SER_HandleChar(char ch)
{
	switch (gState.rx.state)
	{
	case DECODE_START:
		if (ch == START_CHAR)
		{
			gState.rx.state = DECODE_HEADER;
		}
		break;
	case DECODE_HEADER:
	{
		uint8_t len = ch & HEADER_MASK_LEN;
		if (len > sizeof(gState.rx.msg.data))
		{
			gState.rx.state = DECODE_START;
		}
		else
		{
			gState.rx.msg.len = len;
			gState.rx.flags = ch & HEADER_MASK_FLAGS;
			gState.rx.msg.topic = (uint32_t)(ch & HEADER_MASK_TOPIC) << 2;
			gState.rx.state = DECODE_TOPIC;
		}
		break;
	}
	case DECODE_TOPIC:
		gState.rx.msg.topic |= ch;
		gState.rx.state = DECODE_DST;
		break;
	case DECODE_DST:
		gState.rx.msg.dst = ch;
		gState.rx.state = DECODE_DATA;
		if (gState.rx.msg.len == 0)
		{
			SER_HandleMsg(&(gState.rx.msg), gState.rx.flags);
			gState.rx.state = DECODE_START;
		}
		else
		{
			gState.rx.state = DECODE_DATA;
			gState.rx.index = 0;
		}
		break;
	case DECODE_DATA:
		gState.rx.msg.data[gState.rx.index++] = ch;
		if (gState.rx.index >= gState.rx.msg.len)
		{
			SER_HandleMsg(&(gState.rx.msg), gState.rx.flags);
			gState.rx.state = DECODE_START;
		}
		break;
	}
}

void SER_HandleMsg(Msg_t * msg, uint8_t flags)
{
	if (flags & FLAG_TO_LOCAL)
	{
		COMMS_HandleMsg(msg);
	}
	if (flags & FLAG_TO_BUS)
	{
		COMMS_SendMsg(msg);
	}
}

/*
 * INTERRUPT ROUTINES
 */

