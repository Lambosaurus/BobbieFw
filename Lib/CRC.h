#ifndef CRC_H
#define CRC_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

uint32_t CRC32(uint32_t init, uint32_t * words, uint32_t size);

#endif //CRC_H
