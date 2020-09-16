
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

typedef const struct {
	void * ref;
	uint8_t size;
	uint32_t min;
	uint32_t max;
	uint32_t init;
	ConfigEnum_t key;
} ConfigParam_t;

#define CONFIG_PARAM(vkey, member, vinit, vmin, vmax) { \
		.ref = &member,				\
		.size =	sizeof(member),		\
		.min = vmin,				\
		.max = vmax,				\
		.init = vinit,				\
		.key = vkey,				\
	}

/*
 * PRIVATE PROTOTYPES
 */

static uint32_t CFG_PARAM_Get(const ConfigParam_t * p);
static void CFG_PARAM_Set(const ConfigParam_t * p, uint32_t value);
static const ConfigParam_t * CFG_PARAM_Find(ConfigEnum_t en);

/*
 * PRIVATE VARIABLES
 */

Config_t gCfg;

const static ConfigParam_t gParams[] = {
		CONFIG_PARAM(Config_Address, gCfg.address, BOARD_TYPE, 0, 0xFF),
		CONFIG_PARAM(Config_LedAlpha, gCfg.ledAlpha, 127, 0, 0xFF),
		CONFIG_PARAM(Config_ErrorCooldown, gCfg.errorCooldown, 3000, 0, 60000),
		CONFIG_PARAM(Config_ActiveTimeout, gCfg.activeTimeout, 3000, 0, 60000),
		CONFIG_PARAM(Config_TempLimit, gCfg.tempLimit, 80, 0, 200),
		CONFIG_PARAM(Config_FeedbackIdleInterval, gCfg.fbkIdleInterval, 2000, 0, 60000),
		CONFIG_PARAM(Config_FeedbackActiveInterval, gCfg.fbkActiveInterval, 500, 0, 60000),
};

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
	gCfg.version = CONFIG_VERSION;
	for (uint32_t i = 0; i < sizeof(gParams) / sizeof(ConfigParam_t); i++)
	{
		const ConfigParam_t * p = gParams + i;
		CFG_PARAM_Set(p, p->init);
	}
}

bool CFG_Get(ConfigEnum_t en, uint32_t * value)
{
	const ConfigParam_t * p = CFG_PARAM_Find(en);
	if (p != NULL)
	{
		*value = CFG_PARAM_Get(p);
		return true;
	}
	return false;
}

bool CFG_Set(ConfigEnum_t en, uint32_t value)
{
	const ConfigParam_t * p = CFG_PARAM_Find(en);
	if (p != NULL)
	{
		if (value < p->min || value > p->max)
		{
			CFG_PARAM_Set(p, value);
			return true;
		}
	}
	return false;
}


/*
 * PRIVATE FUNCTIONS
 */

static uint32_t CFG_PARAM_Get(const ConfigParam_t * p)
{
	switch (p->size)
	{
	default:
	case 1:
		return *(uint8_t*)p->ref;
	case 2:
		return *(uint16_t*)p->ref;
	case 4:
		return *(uint32_t*)p->ref;
	}
}

static void CFG_PARAM_Set(const ConfigParam_t * p, uint32_t value)
{
	switch (p->size)
	{
	default:
	case 1:
		*(uint8_t*)p->ref = value;
		break;
	case 2:
		*(uint16_t*)p->ref = value;
		break;
	case 4:
		*(uint32_t*)p->ref = value;
		break;
	}
}

static const ConfigParam_t * CFG_PARAM_Find(ConfigEnum_t en)
{
	for (uint32_t i = 0; i < sizeof(gParams) / sizeof(ConfigParam_t); i++)
	{
		const ConfigParam_t * p = gParams + i;
		if (p->key == en)
		{
			return p;
		}
	}
	return NULL;
}


/*
 * INTERRUPT ROUTINES
 */
