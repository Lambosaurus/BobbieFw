
#include "Config.h"
#include "E3PROM.h"
#include <string.h>
#include "LED.h"
#include "Messages.h"

/*
 * PRIVATE DEFINITIONS
 */

#define CONFIG_VERSION 0x0005

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

/*
 * PRIVATE VARIABLES
 */

Config_t gCfg;

/*
 * PUBLIC FUNCTIONS
 */


void CFG_Load(void)
{
	if (!E3PROM_Read(&gCfg, sizeof(gCfg)) || gCfg.version != CONFIG_VERSION)
	{
		CFG_Default();
	}
}

void CFG_Save(void)
{
	E3PROM_Write(&gCfg, sizeof(gCfg));
}

void CFG_Default(void)
{
	Config_t cfg = {
		.version = CONFIG_VERSION,
		.address = BOARD_TYPE,
		.ledAlpha = 127,	 // Half power
		.errorCooldown = 30, // 3s
		.activeTimeout = 10, // 1s
#ifdef SER_USE_BRIDGE
		.serialBridge = SerialBridge_None,
#endif
		.tempLimit = 80,
		.fbkIdleInterval = 20,
		.fbkActiveInterval = 5,
	};
	memcpy(&gCfg, &cfg, sizeof(gCfg));
}

bool CFG_Get(ConfigEnum_t en, uint32_t * value)
{
	switch (en)
	{
	case Config_Address:
		*value = gCfg.address;
		return true;
	case Config_LedAlpha:
		*value = gCfg.ledAlpha;
		return true;
	case Config_ErrorCooldown:
		*value = gCfg.errorCooldown;
		return true;
	case Config_ActiveTimeout:
		*value = gCfg.activeTimeout;
		return true;
#ifdef SER_USE_BRIDGE
	case Config_SerialBridge:
		*value = gCfg.serialBridge;
		return true;
#endif
	case Config_TempLimit:
		*value = gCfg.tempLimit;
		return true;
	case Config_FeedbackIdleInterval:
		*value = gCfg.fbkIdleInterval;
		return true;
	case Config_FeedbackActiveInterval:
		*value = gCfg.fbkActiveInterval;
		return true;
	}
	return false;
}

bool CFG_Set(ConfigEnum_t en, uint32_t value)
{
	switch (en)
	{
	case Config_Address:
		if (value > 0)
		{
			gCfg.address = value;
			return true;
		}
		break;
	case Config_LedAlpha:
		if (value <= 255)
		{
			gCfg.ledAlpha = value;
			return true;
		}
		break;
	case Config_ErrorCooldown:
		gCfg.errorCooldown = value;
		return true;
	case Config_ActiveTimeout:
		gCfg.activeTimeout = value;
		return true;
#ifdef SER_USE_BRIDGE
	case Config_SerialBridge:
		if (value <= SerialBridge_All)
		{
			gCfg.serialBridge = value;
			return true;
		}
		break;
#endif
	case Config_TempLimit:
		gCfg.tempLimit = value;
		return true;
	case Config_FeedbackIdleInterval:
		gCfg.fbkIdleInterval = value;
		return true;
	case Config_FeedbackActiveInterval:
		gCfg.fbkActiveInterval = value;
		return true;
	}
	return false;
}


/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
