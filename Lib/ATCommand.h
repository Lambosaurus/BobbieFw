/********************************************************************
*
*        AT Command handler
*
*********************************************************************
* Processor Specific:   STM32L0...
* Complier Specific:    SW4STM32 and STM32 Cube HAL
*
* NOTE:					Notes or none
*
* Copyright (c) 2017, Infact Ltd.  All rights reserved.
*
********************************************************************/
			   
#ifndef ATCOMMAND_H
#define ATCOMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define AT_DATA_INT 		AT_Int
#define AT_DATA_STR 		AT_Str

#ifndef AT_DATA_SIZE_MAX
#define AT_DATA_SIZE_MAX	64
#endif

#define AT_LF				"\r\n"

typedef enum {
	AT_Error,
	AT_Ok,
	AT_Str,
	AT_Int,
	AT_BadReg,
	AT_BadValue,
	AT_BadSyntax,
	AT_BadCommand,
	AT_CannotRead,
	AT_CannotWrite,
#ifdef AT_EXEC_SUPPORT
	AT_CannotExec,
#endif
	AT_NoReply,
} ATReply_t;

typedef union {
	int number;
	char string[AT_DATA_SIZE_MAX];
} ATValue_t;

typedef struct {
	char * Prefix;
	uint8_t DataType;
	ATReply_t (*Get)(int reg, ATValue_t * value);
	ATReply_t (*Set)(int reg, ATValue_t * value);
#ifdef AT_EXEC_SUPPORT
	ATReply_t (*Exec)(int reg, ATValue_t * value);
#endif
} ATCommand_t;

typedef struct {
	ATCommand_t * Commands;
	int CommandCount;
	char * Prefix;
	int PrefixLen;
} ATHandler_t;

char * AT_HandleLine(ATHandler_t * handler, char * input, char * buffer, int16_t bufferSize);


#ifdef __cplusplus
}
#endif
#endif


