
#include "CAN.h"
#include "stm32f0xx_hal_can.h"

/*
 * PRIVATE DEFINITIONS
 */

#define CAN_TIME_QUANTA 	16u
#define CAN_BITRATE			250000u

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

/*
 * PRIVATE VARIABLES
 */

static CAN_HandleTypeDef gCan;

/*
 * PUBLIC FUNCTIONS
 */

void CAN_Init(void)
{
	__HAL_RCC_CAN1_CLK_ENABLE();

	GPIO_InitTypeDef gpio = {0};
	gpio.Pin = CAN_PINS;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio.Alternate = GPIO_AF4_CAN;
	HAL_GPIO_Init(CAN_GPIO, &gpio);

	// This is an even 8 for 32MHz, and 12 for 48MHz
	uint32_t prescalar = HAL_RCC_GetPCLK1Freq() / (CAN_BITRATE * CAN_TIME_QUANTA);

	gCan.Instance = CAN;
	gCan.Init.Prescaler = prescalar;
	gCan.Init.Mode = CAN_MODE_NORMAL;
	gCan.Init.SyncJumpWidth = CAN_SJW_2TQ;
	gCan.Init.TimeSeg1 = CAN_BS1_13TQ;
	gCan.Init.TimeSeg2 = CAN_BS2_2TQ;
	gCan.Init.TimeTriggeredMode = DISABLE;
	gCan.Init.AutoBusOff = DISABLE; //ENABLE;
	gCan.Init.AutoWakeUp = ENABLE;
	gCan.Init.AutoRetransmission = ENABLE; //DISABLE;
	gCan.Init.ReceiveFifoLocked = DISABLE;
	gCan.Init.TransmitFifoPriority = DISABLE;
	HAL_CAN_Init(&gCan);


	CAN_FilterTypeDef filter =
	{
		.FilterFIFOAssignment = CAN_FILTER_FIFO0,
		.FilterIdHigh = 0,
		.FilterIdLow = 0,
		.FilterMaskIdHigh = 0,
		.FilterMaskIdLow = 0,
		.FilterScale = CAN_FILTERSCALE_32BIT,
		.FilterActivation = CAN_FILTER_ENABLE,
		.FilterBank = 1,
		.FilterMode = CAN_FILTERMODE_IDMASK,
	};
	HAL_CAN_ConfigFilter(&gCan, &filter);

	HAL_CAN_Start(&gCan);
}

void CAN_Deinit(void)
{
	HAL_CAN_Stop(&gCan);
	HAL_CAN_DeInit(&gCan);
	__HAL_RCC_CAN1_CLK_DISABLE();
	HAL_GPIO_DeInit(CAN_GPIO, CAN_PINS);
}

bool CAN_Tx(CANMsg_t * msg)
{
	CAN_TxHeaderTypeDef tx = {
			.ExtId = msg->id,
			.IDE = CAN_ID_EXT,
			.RTR = CAN_RTR_DATA,
			.DLC = msg->len,
	};
	uint32_t mailbox;
	return HAL_CAN_AddTxMessage(&gCan, &tx, msg->data, &mailbox) == HAL_OK;
}


uint8_t CAN_RxCount(void)
{
	//HAL_CAN_GetRxFifoFillLevel(&gCan, CAN_RX_FIFO0);
	return gCan.Instance->RF0R & CAN_RF0R_FMP0;
}

bool CAN_Rx(CANMsg_t * msg)
{
	if (CAN_RxCount())
	{
		CAN_RxHeaderTypeDef rx;
		if ( HAL_CAN_GetRxMessage(&gCan, CAN_RX_FIFO0, &rx, msg->data) == HAL_OK)
		{
			msg->len = rx.DLC;
			switch (rx.IDE)
			{
			case CAN_ID_EXT:
				msg->id = rx.ExtId;
				break;
			default:
				msg->id = rx.StdId;
				break;
			}
			return true;
		}
	}
	return false;
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
