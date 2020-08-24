
#include "Serial.h"
#include "UART.h"

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

void SER_ReadLine(void);
void SER_HandleLine(char * line);

/*
 * PRIVATE VARIABLES
 */

static struct {
	char bfr[64];
	uint16_t read;
} gState;

/*
 * PUBLIC FUNCTIONS
 */

void SER_Init(void)
{
	UART_Init(SER_UART, 115200);
	gState.read = 0;
}

void SER_Deinit(void)
{
	UART_Deinit(SER_UART);
}

void SER_Update(State_t state)
{
	SER_ReadLine();
}

/*
 * PRIVATE FUNCTIONS
 */

void SER_ReadLine(void)
{
	uint16_t ready = UART_RxCount(SER_UART);
	while (ready--)
	{
		char ch = UART_RxPop(SER_UART);
		if (ch == '\n')
		{
			gState.bfr[gState.read] = 0;
			SER_HandleLine(gState.bfr);
			gState.read = 0;
		}
		else if (gState.read < sizeof(gState.bfr) - 1)
		{
			gState.bfr[gState.read++] = ch;
		}
	}
}

void SER_HandleLine(char * line)
{
	UART_TxStr(SER_UART, line);
}

/*
 * INTERRUPT ROUTINES
 */

