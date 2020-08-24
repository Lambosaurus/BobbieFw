
/********************************************************************
*
*     Modules Used
*
********************************************************************/

#include "ATCommand.h"
#include <stdio.h>
#include <string.h>

/********************************************************************
*
*     Private Definitions and Macro's
*
********************************************************************/

/********************************************************************
*
*     Private Types
*
********************************************************************/

/********************************************************************
*
*     Private Function Prototypes
*
********************************************************************/

static ATReply_t AT_GetResponse(ATHandler_t * handler, char * line, ATValue_t * value);
static char* Str_Atoi(char* str, int* value);

/********************************************************************
*
*     Public Global Variables
*
********************************************************************/

char * AT_HandleLine(ATHandler_t * handler, char * input, char * buffer, int16_t bufferSize)
{
	ATValue_t value;
	ATReply_t reply = AT_GetResponse(handler, input, &value);

	switch (reply)
	{
	case AT_Error:
		return "Error" AT_LF;
	case AT_Ok:
		return "OK" AT_LF;
	case AT_Int:
		snprintf( buffer, bufferSize, "%d" AT_LF, value.number);
		return buffer;
	case AT_Str:
		snprintf( buffer, bufferSize, "%s" AT_LF, value.string);
		return buffer;
	case AT_BadReg:
		return "Bad Register" AT_LF;
	case AT_BadValue:
		return "Bad Value" AT_LF;
	case AT_BadSyntax:
		return "Bad Syntax" AT_LF;
	case AT_BadCommand:
		return "Bad Command" AT_LF;
	case AT_CannotRead:
		return "Register cannot be read" AT_LF;
	case AT_CannotWrite:
		return "Register cannot be written" AT_LF;
#ifdef AT_EXEC_SUPPORT
	case AT_CannotExec:
		return "Register cannot be executed" AT_LF;
#endif AT_EXEC_SUPPORT
	case AT_NoReply:
		return "";
	default:
		return "Unhandled Error" AT_LF;
	}
}

/********************************************************************
*
*     Private Global Variables
*
********************************************************************/

static char* Str_Atoi(char* str, int* value)
{
	int sign = 1;
	int v = 0;

	char c = *str;
	if (c == '-')
	{
		sign = -1;
		c = *(++str);
	}

	while (c >= '0' && c <= '9')
	{
		v = (v * 10) + c - '0';
		c = *(++str);
	}

	*value = v * sign;
	return str;
}

static inline bool AT_IsPrefixChar(char ch)
{
	return ch >= 'A' && ch <= 'z';
}

static inline bool AT_IsChainingChar(char ch)
{
	return ch == '&';
}

static inline bool AT_IsEndOfLine(char ch)
{
	return ch == 0 || ch == '\r' || ch == '\n' || ch == ';';
}

static ATReply_t AT_GetResponse(ATHandler_t * handler, char * line, ATValue_t * value)
{
	char * str = line;

	// Check the prefix, and advance.
	if (strncmp( str, handler->Prefix, handler->PrefixLen ) != 0)
	{
		// Do not reply.
		return AT_NoReply;
	}
	str += handler->PrefixLen;

	// Find the prefix string
	char * prefix = str;
	while ( AT_IsPrefixChar(*str) ) { str++; }
	int prefixLen = str - prefix;

	if (prefixLen <= 0)
	{
		return AT_BadSyntax;
	}

	// Match to the command
	ATCommand_t * cmd = NULL;
	for (int i = 0; i < handler->CommandCount; i++)
	{
		if (strncmp(handler->Commands[i].Prefix, prefix, prefixLen) == 0)
		{
			cmd = &(handler->Commands[i]);
			break;
		}
	}
	if (cmd == NULL)
	{
		return AT_BadCommand;
	}

	// If the number is unspecified and the reg will default to 0.
	// We can test for this by checking that str has not advanced if we need to.
	int reg;
	str = Str_Atoi(str, &reg);

	char operator = *str++;

	if (operator == '=')
	{
		if (cmd->Set != NULL)
		{
			switch (cmd->DataType)
			{
			case AT_DATA_INT:
			{
				char * intStart = str;
				str = Str_Atoi(str, &(value->number));
				int intLength = str - intStart;
				if (intLength < 1)
				{
					return AT_BadValue;
				}
				break;
			}
			case AT_DATA_STR:
			{
				// Copy the string into the buffer so it can be null terminated.
				int strLength = 0;
				while (!AT_IsEndOfLine(*str) && strLength < AT_DATA_SIZE_MAX - 1)
				{
					value->string[strLength++] = *str++;
				}
				value->string[strLength] = 0;
				break;
			}
			default:
				return AT_CannotWrite;
			}

			return cmd->Set(reg, value);
		}
		return AT_CannotWrite;
	}
	else if (operator == '?')
	{
		if (cmd->Get != NULL)
		{
			return cmd->Get(reg, value);
		}
		return AT_CannotRead;
	}
#ifdef AT_EXEC_SUPPORT
	else if (AT_IsEndOfLine(operator))
	{
		if (cmd->Exec != NULL)
		{
			return cmd->Exec(reg, value);
		}
		return AT_CannotExec;
	}
#endif
	else
	{
		return AT_BadSyntax;
	}
}

/********************************************************************
*
*     Public Function Definitions
*
********************************************************************/

/********************************************************************
*
*     Private Function Definitions
*
********************************************************************/


/********************************************************************
*
*    Interrupt Service Routines
*
********************************************************************/

/********************************************************************
*
*     EOF
*
********************************************************************/









