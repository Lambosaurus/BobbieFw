
#include "Messages.h"
#include "Bus.h"
#include "Serial.h"
#include "UART.h"
#include "Config.h"
#include <string.h>

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

#define SER_STRAT_NEW

#ifdef SER_STRAT_NEW
typedef enum {
	DECODE_START,
	DECODE_HEADER,
	DECODE_DATA,
} DecodeState_t;
#else
typedef enum {
	DECODE_START,
	DECODE_HEADER,
	DECODE_TOPIC,
	DECODE_DST,
	DECODE_DATA,
} DecodeState_t;
#endif


#define SERIAL_TIMEOUT_MS		100

#define SERIAL_START_CHAR 		0x5F
#define HEADER_MASK_LEN			0x0F
#define HEADER_MASK_TOPIC		0xC0
#define HEADER_MASK_FLAGS		0x30
#define SERIAL_FLAG_TOLOCAL		0x10


/*
 * PRIVATE PROTOTYPES
 */

static void SER_Read(void);
#ifdef SER_STRAT_NEW
static void SER_ParseMsg(uint8_t * bfr, uint8_t length);
#endif
static void SER_HandleChar(uint8_t ch);

/*
 * PRIVATE VARIABLES
 */

#ifdef SER_STRAT_NEW
static struct {
	struct {
		uint16_t index;
		DecodeState_t state;
		uint8_t bfr[SERIAL_SIZE_MAX];
		uint8_t length;
		uint32_t timeout;
	} rx;
} gState;
#else
static struct {
	struct {
		uint16_t index;
		DecodeState_t state;
		Msg_t msg;
		uint32_t timeout;
	}rx;
} gState;
#endif

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

#ifdef SER_USE_BRIDGE
void SER_TxMsg(Msg_t * msg)
{
	uint8_t bfr[SERIAL_SIZE_MAX];
	bfr[0] = SERIAL_START_CHAR;
	bfr[1] = (uint8_t)((msg->topic >> 2) & HEADER_MASK_TOPIC) | (msg->len);
	bfr[2] = msg->src;
	bfr[3] = (uint8_t)(msg->topic);
	memcpy(bfr + SERIAL_SIZE_HEADER, msg->data, msg->len);
	UART_Tx(SER_UART, bfr, SERIAL_SIZE_HEADER + msg->len);
}
#endif

/*
 * PRIVATE FUNCTIONS
 */

static void SER_Read(void)
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

#ifdef SER_STRAT_NEW


static void SER_HandleChar(uint8_t ch)
{
	switch (gState.rx.state)
	{
	case DECODE_START:
		if (ch == SERIAL_START_CHAR)
		{
			gState.rx.state = DECODE_HEADER;
		}
		break;
	case DECODE_HEADER:
	{
		uint8_t len = ch & HEADER_MASK_LEN;
		if (len > SERIAL_SIZE_DATA)
		{
			gState.rx.state = DECODE_START;
		}
		else
		{
			gState.rx.state = DECODE_DATA;
			gState.rx.bfr[1] = ch;
			gState.rx.index = 2; // Skip the start char and header.
			gState.rx.length = len + SERIAL_SIZE_HEADER;
		}
		break;
	}
	case DECODE_DATA:
		gState.rx.bfr[gState.rx.index++] = ch;
		if (gState.rx.index >= gState.rx.length)
		{
			SER_ParseMsg(gState.rx.bfr, gState.rx.length);
			gState.rx.state = DECODE_START;
		}
		break;
	}
}

static void SER_ParseMsg(uint8_t * bfr, uint8_t length)
{
	uint8_t header = bfr[1];
	Msg_t msg = {
		.src = gCfg.address,
		.dst = (header & SERIAL_FLAG_TOLOCAL) ? bfr[3] : gCfg.address,
		.topic = bfr[2] | ((uint32_t)(header & HEADER_MASK_TOPIC) << 2),
		.len = length - SERIAL_SIZE_HEADER,
	};
	memcpy(msg.data, bfr+SERIAL_SIZE_HEADER, msg.len);
	MSG_Handle(&msg, MsgSrc_Serial);
}

#else
static void SER_HandleChar(uint8_t ch)
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
			SER_HandleMsg(&(gState.rx.msg));
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
			SER_HandleMsg(&(gState.rx.msg));
			gState.rx.state = DECODE_START;
		}
		break;
	}
}
#endif


/*
 * INTERRUPT ROUTINES
 */

