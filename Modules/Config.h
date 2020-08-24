#ifndef CONFIG_H
#define CONFIG_H

#include "State.h"
#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

// Care to word align this..
typedef struct {
	uint8_t address;
	uint8_t ledAlpha;			// 0 - 16
	uint8_t errorCooldown;		// 100ms steps
	uint8_t activeTimeout; 		// 100ms steps
} __attribute__((aligned(4))) Config_t;

extern Config_t gCfg;
/*
 * PUBLIC FUNCTIONS
 */

void CFG_Load(void);
void CFG_Save(void);
void CFG_Default(void);

#endif //CONFIG_H
