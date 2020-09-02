
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

typedef enum {
	DECODE_START,
	DECODE_HEADER,
	DECODE_DATA,
} DecodeState_t;


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
static void SER_ParseMsg(uint8_t * bfr, uint8_t length);
static void SER_HandleChar(uint8_t ch);

/*
 * PRIVATE VARIABLES
 */

static struct {
	struct {
		uint16_t index;
		DecodeState_t state;
		uint8_t bfr[SERIAL_SIZE_MAX];
		uint8_t length;
		uint32_t timeout;
	} rx;
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

#ifdef SER_USE_BRIDGE
void SER_TxMsg(Msg_t * msg)
{
	uint8_t bfr[SERIAL_SIZE_MAX];
	bfr[0] = SERIAL_START_CHAR;
	bfr[1] = (uint8_t)((msg->topic >> 2) & HEADER_MASK_TOPIC) | (msg->len);
	bfr[2] = (uint8_t)(msg->topic);
	bfr[3] = msg->src;
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
		.dst = (header & SERIAL_FLAG_TOLOCAL) ? gCfg.address : bfr[3],
		.topic = bfr[2] | ((uint32_t)(header & HEADER_MASK_TOPIC) << 2),
		.len = length - SERIAL_SIZE_HEADER,
		.prio = 2,
	};
	memcpy(msg.data, bfr+SERIAL_SIZE_HEADER, msg.len);
	MSG_Handle(&msg, MsgSrc_Serial);
}

/*
 * INTERRUPT ROUTINES
 */

