
#include "UART.h"
#include "Core.h"
#include "string.h"

/*
 * PRIVATE DEFINITIONS
 */

#define UART_BFR_INCR(v) ((v + 1) & (UART_BFR_SIZE - 1))

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void UARTx_Init(UART_t * uart);
static void UARTx_Deinit(UART_t * uart);

/*
 * PRIVATE VARIABLES
 */

#ifdef UART1_GPIO
static UART_t gUART_1 = {
	.Instance = USART1
};
UART_t * UART_1 = &gUART_1;
#endif
#ifdef UART2_GPIO
static UART_t gUART_2 = {
	.Instance = USART2
};
UART_t * UART_2 = &gUART_2;
#endif

/*
 * PUBLIC FUNCTIONS
 */


void UART_Init(UART_t * uart, uint32_t baud)
{
	uart->tx.head = uart->tx.tail = 0;
	uart->rx.head = uart->rx.tail = 0;

	// Enable the uart specific GPIO and clocks.
	UARTx_Init(uart);

	__HAL_UART_DISABLE(uart);
	// Configure to standard settings: 8N1, no flow control.
	uint32_t cr1 = (uint32_t)UART_WORDLENGTH_8B | UART_PARITY_NONE | UART_MODE_TX_RX | UART_OVERSAMPLING_16;
	MODIFY_REG(uart->Instance->CR1, USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8, cr1);
	MODIFY_REG(uart->Instance->CR2, USART_CR2_STOP, UART_STOPBITS_1);
	uint32_t cr3 = (uint32_t)UART_HWCONTROL_NONE | UART_ONE_BIT_SAMPLE_DISABLE;
	MODIFY_REG(uart->Instance->CR3, (USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT), cr3);

	// Calculate baud rate.
	uint32_t pclk = HAL_RCC_GetPCLK1Freq();
	uart->Instance->BRR = UART_DIV_SAMPLING16(pclk, baud);

	CLEAR_BIT(uart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
	CLEAR_BIT(uart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));
	__HAL_UART_ENABLE(uart);

	// Enable RX IRQ.
	uart->Instance->CR1 |= USART_CR1_RXNEIE;
}

void UART_Deinit(UART_t * uart)
{
	// Disable RX IRQ, and TX IRQ in case a tx is underway.
	uart->Instance->CR1 &= ~(USART_CR1_RXNEIE | USART_CR1_TXEIE);

	__HAL_UART_DISABLE(uart);
	// Clear all control registers.
	uart->Instance->CR1 = 0x0U;
	uart->Instance->CR2 = 0x0U;
	uart->Instance->CR3 = 0x0U;

	// Disable uart specific GPIO and clocks.
	UARTx_Deinit(uart);
}

void UART_Tx(UART_t * uart, uint8_t * data, uint16_t count)
{
	while (count--)
	{
		// calculate the next head. We cant assign it yet, as the IRQ relies on it.
		uint16_t head = UART_BFR_INCR(uart->tx.head);

		// If the head has caught up with tail.. wait.
		while (head == uart->tx.tail) { CORE_Idle(); }

		uart->tx.buffer[uart->tx.head] = *data++;
		uart->tx.head = head;

		// Enable transmitter - no guarantee it was ever enabled.
		uart->Instance->CR1 |= USART_CR1_TXEIE;
	}
}

void UART_TxStr(UART_t * uart, char * str)
{
	UART_Tx(uart, (uint8_t *)str, strlen(str));
}

uint16_t UART_RxCount(UART_t * uart)
{
	__disable_irq();
	// We have to disable the IRQs, as the IRQ may bump the tail.
	uint16_t count = uart->rx.head >= uart->rx.tail
				   ? uart->rx.head - uart->rx.tail
				   : UART_BFR_SIZE + uart->rx.head - uart->rx.tail;
	__enable_irq();
	return count;
}

uint16_t UART_Rx(UART_t * uart, uint8_t * data, uint16_t count)
{
	uint16_t available = UART_RxCount(uart);
	if (available < count)
	{
		count = available;
	}

	// Copy the bytes out with the IRQ disabled the whole time.
	// This could be done with successive UART_Pop() calls, but this should be a pinch faster.
	__disable_irq();
	uint16_t tail = uart->rx.tail;
	for (uint16_t i = 0; i < count; i++)
	{
		*data++ = uart->rx.buffer[tail];
		tail = UART_BFR_INCR(tail);
	}
	uart->rx.tail = tail;
	__enable_irq();

	return count;
}

uint8_t UART_Pop(UART_t * uart)
{
	__disable_irq();
	uint8_t b = uart->rx.buffer[uart->rx.tail];
	uart->rx.tail = UART_BFR_INCR(uart->rx.tail);
	__enable_irq();
	return b;
}

void UART_RxClear(UART_t * uart)
{
	__disable_irq();
	uart->rx.tail = uart->rx.head;
	__enable_irq();
}

/*
 * PRIVATE FUNCTIONS
 */

static void UARTx_Init(UART_t * uart)
{
	GPIO_InitTypeDef gpio = {0};
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;


#ifdef UART1_GPIO
	if (uart == UART_1)
	{
		__HAL_RCC_USART1_CLK_ENABLE();
		gpio.Pin = UART1_PINS;
		gpio.Alternate = GPIO_AF1_USART1;
		HAL_GPIO_Init(UART1_GPIO, &gpio);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}
#endif
#ifdef UART2_GPIO
	if (uart == UART_2)
	{
		__HAL_RCC_USART2_CLK_ENABLE();
		gpio.Pin = UART2_PINS;
		gpio.Alternate = GPIO_AF1_USART2;
		HAL_GPIO_Init(UART2_GPIO, &gpio);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}
#endif
}

static void UARTx_Deinit(UART_t * uart)
{
#ifdef UART1_GPIO
	if (uart == UART_1)
	{
		HAL_NVIC_DisableIRQ(USART1_IRQn);
		__HAL_RCC_USART1_CLK_DISABLE();
		HAL_GPIO_DeInit(UART1_GPIO, UART1_PINS);
	}
#endif
#ifdef UART2_GPIO
	if (uart == UART_2)
	{
		HAL_NVIC_DisableIRQ(USART2_IRQn);
		__HAL_RCC_USART2_CLK_DISABLE();
		HAL_GPIO_DeInit(UART2_GPIO, UART2_PINS);
	}
#endif
}

/*
 * INTERRUPT ROUTINES
 */


void USART_IRQHandler(UART_t *uart)
{
	uint32_t flags = uart->Instance->ISR;

	if (flags & USART_ISR_RXNE)
	{
		// New RX data. Put it in the RX buffer.
		uart->rx.buffer[uart->rx.head] = uart->Instance->RDR;
		uart->rx.head = UART_BFR_INCR(uart->rx.head);
		if (uart->rx.head == uart->rx.tail) {
			// The head just caught up with the tail. Uh oh. Increment the tail.
			// Note, this causes flaming huge issues.
			uart->rx.tail = UART_BFR_INCR(uart->rx.tail);
		}
	}

	if (flags & USART_ISR_TXE)
	{
		// No byte being transmitted..
		if (uart->tx.head != uart->tx.tail)
		{
			// Send a byte out.
			uart->Instance->TDR = uart->tx.buffer[uart->tx.tail];
			uart->tx.tail = UART_BFR_INCR(uart->tx.tail);
		}
		else
		{
			// Tail caught up with head: no bytes remain.
			// Disable the TX IRQ.
			uart->Instance->CR1 &= ~USART_CR1_TXEIE;
		}
	}
}


#ifdef UART1_GPIO
void USART1_IRQHandler(void)
{
	USART_IRQHandler(UART_1);
}
#endif
#ifdef UART2_GPIO
void USART2_IRQHandler(void)
{
	USART_IRQHandler(UART_2);
}
#endif


