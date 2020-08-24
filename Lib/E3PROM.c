
#include "E3PROM.h"
#include "Core.h"
#include "CRC.h"
#include <string.h>

/*
 * PRIVATE DEFINITIONS
 */

#define E3PROM_PAGE 	(FLASH_BANK1_END - FLASH_PAGE_SIZE + 1)

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void FLASH_PageErase(uint32_t PageAddress);
static bool FLASH_WaitForOperation(uint32_t timeout);
static void FLASH_ProgramWord(uint32_t address, uint32_t data);

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */


bool E3PROM_Read(void * data, uint16_t size)
{
	uint32_t * words = (uint32_t *)data;
	uint32_t address = E3PROM_PAGE;

	uint32_t crc = *(uint32_t*)address;
	memcpy(words, (uint32_t *)(address + 4), size);

	uint32_t actual = CRC32(0, words, size);
	return actual == crc;
}

void E3PROM_Write(void * data, uint16_t size)
{
	uint32_t * words = (uint32_t*)data;

	uint32_t crc = CRC32(0, words, size);
	uint32_t address = E3PROM_PAGE;


	if (crc == *(uint32_t*)address)
	{
		// No change, so skip the writing process
		return;
	}

	HAL_FLASH_Unlock();

	FLASH_PageErase(address);
	FLASH_ProgramWord(address, crc);

	for (uint16_t i = 0; i < size/4; i++)
	{
		address += 4;
		FLASH_ProgramWord(address, words[i]);
	}

	HAL_FLASH_Lock();
}

/*
 * PRIVATE FUNCTIONS
 */

void FLASH_PageErase(uint32_t PageAddress)
{
    SET_BIT(FLASH->CR, FLASH_CR_PER);
    WRITE_REG(FLASH->AR, PageAddress);
    SET_BIT(FLASH->CR, FLASH_CR_STRT);
    FLASH_WaitForOperation(100);
	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
}

bool FLASH_WaitForOperation(uint32_t timeout)
{
	uint32_t tickstart = HAL_GetTick();
	while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
	{
		if((HAL_GetTick() - tickstart) > timeout)
		{
			return false;
		}
	}
	if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP))
	{
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
	}
	if(__HAL_FLASH_GET_FLAG(FLASH_FLAG_WRPERR) || __HAL_FLASH_GET_FLAG(FLASH_FLAG_PGERR))
	{
		return false;
	}
	return true;
}

static void FLASH_ProgramWord(uint32_t address, uint32_t data)
{
	uint16_t * words = (uint16_t *)(&data);

    SET_BIT(FLASH->CR, FLASH_CR_PG);
	*(__IO uint16_t*)(address+0) = (uint16_t)words[0];
	FLASH_WaitForLastOperation(100);

	*(__IO uint16_t*)(address+2) = (uint16_t)words[1];
	FLASH_WaitForLastOperation(100);
	CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
}

/*
 * INTERRUPT ROUTINES
 */

