#ifndef E3PROM_H
#define E3PROM_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

#define E3PROM_SIZE			(FLASH_PAGE_SIZE - 4)

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

bool E3PROM_Read(void * data, uint16_t size);
void E3PROM_Write(void * data, uint16_t size);

/*
 * EXTERN DECLARATIONS
 */

#endif //E3PROM_H
