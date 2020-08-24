
#include "Config.h"
#include "E3PROM.h"
#include <string.h>

/*
 * PRIVATE DEFINITIONS
 */

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
	if (!E3PROM_Read(&gCfg, sizeof(gCfg)))
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
		.address = 1
	};
	memcpy(&gCfg, &cfg, sizeof(gCfg));
}


/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
