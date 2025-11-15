#include <stdio.h>
#include <stdlib.h>
#include "util.h"

typedef enum Opcode
{
	OPCODE_NOOP,
	OPCODE_INDEX,
	OPCODE_POS,
	OPCODE_ADD,
	OPCODE_MULT,
	OPCODECOUNT, // amount of opcodes defined
} Opcode;

const char* opcodeSymbols[OPCODECOUNT] = {
	"noop",
	"@",
	"@@",
	"+",
	"*",
};

const char* opcodeNames[OPCODECOUNT] =
{
	"NOOP",
	"INDEX",
	"POS",
	"ADD",
	"MULT",
};

typedef enum TokenType
{
	TOKENTYPE_OPCODE,
	TOKENTYPE_LITERAL,
} TokenType;

typedef struct Token
{
	TokenType type;
	union
	{
		double literal;
		Opcode opcode;
	} value;
	struct Token* next;
} Token;

void initToken (Token* token)
{
	token->type = TOKENTYPE_OPCODE;
	token->value.opcode = OPCODE_NOOP;
	token->next = NULL;
}

void freeToken (Token* token)
{
	if(token == NULL) { return; }
	freeToken(token->next);
	free(token);
}

void printToken (Token* token)
{	
	while(token != NULL)
	{
		switch(token->type)
		{
			case TOKENTYPE_LITERAL :
			{
				printf("LITERAL %lf -> ", token->value.literal);
				break;
			}
			case TOKENTYPE_OPCODE :
			{
				printf("OPCODE %s -> ", opcodeNames[(int) token->value.opcode]);
				break;
			}
			default :
			{
				printf("INVALID -> ");
			}
		}
		token = token->next;
	}
	printf("NULL\n");
}

void tokenizeFormula (char* s, Token** token)
{
	if(*token != NULL)
	{
		freeToken(*token);
		*token = NULL;
	}
	Token* tail = *token;
	
	char build[64];
	unsigned int buildLength = 0;
	int i = 0;
	while(1)
	{
		if(s[i] == ' ' || s[i] == '\0')
		{
			if(buildLength > 0)
			{
				build[buildLength] = '\0';
				buildLength = 0;
				
				if(*token == NULL)
				{
					tail = (Token*) malloc(sizeof(Token));
					initToken(tail);
					*token = tail;
				}
				else
				{
					tail->next = (Token*) malloc(sizeof(Token));
					tail = tail->next;
					initToken(tail);
				}
				
				int opcodeFound = 0;
				for(int i = 0; i < OPCODECOUNT; i++)
				{
					if(stringEquals(build, opcodeSymbols[i]))
					{
						tail->value.opcode = (Opcode) i;
						opcodeFound = 1;
						break;
					}
				}
				if(!opcodeFound)
				{
					int result = sscanf(build, "%lf", &tail->value.literal);
					if(result > 0)
					{
						tail->type = TOKENTYPE_LITERAL;
					}
				}
			}
			if(s[i] == '\0') { return; }
		}
		else
		{
			build[buildLength] = s[i];
			buildLength += 1;
		}
		i += 1;
	}
}