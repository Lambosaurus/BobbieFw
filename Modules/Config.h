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

typedef enum {
	Config_Address,
	Config_LedAlpha,
	Config_ErrorCooldown,
	Config_ActiveTimeout,
	Config_SerialBridge,
	Config_TempLimit,
	Config_FeedbackIdleInterval,
	Config_FeedbackActiveInterval,
} ConfigEnum_t;

typedef enum {
	SerialBridge_None = 0,
	SerialBridge_Local,
	SerialBridge_All
} SerialBridgeMode_t;

// Care to word align this..
typedef struct {
	uint16_t version;
	uint8_t address;
	uint8_t ledAlpha;			// 0 - 255
	uint16_t errorCooldown;		// ms
	uint16_t activeTimeout; 	// ms
#ifdef SER_USE_BRIDGE
	uint8_t serialBridge;
#endif
	uint8_t tempLimit;
	uint16_t fbkIdleInterval;   // ms
	uint16_t fbkActiveInterval; // ms
} __attribute__((aligned(4))) Config_t;

extern Config_t gCfg;
/*
 * PUBLIC FUNCTIONS
 */

void CFG_Load(void);
void CFG_Save(void);
void CFG_Default(void);

bool CFG_Get(ConfigEnum_t en, uint32_t * value);
bool CFG_Set(ConfigEnum_t en, uint32_t value);

#endif //CONFIG_H
