#include <stdio.h>
#include <stdlib.h>
#include "util.h"

extern const unsigned int ALPHABETSIZE;
extern const char ALPHABET[];

typedef enum Opcode
{
	OPCODE_EMPTY,
	OPCODE_INDEX,
	OPCODE_POS,
	OPCODE_ADD,
	OPCODE_SUB,
	OPCODE_MULT,
	OPCODE_DIV,
	OPCODE_SUM,
	OPCODECOUNT, // amount of opcodes defined
} Opcode;

const char* opcodeSymbols[OPCODECOUNT] = {
	"EMPTY",
	"@",
	"@@",
	"+",
	"-",
	"*",
	"/",
	"SUM",
};

const char* opcodeNames[OPCODECOUNT] =
{
	"EMPTY",
	"INDEX",
	"POS",
	"ADD",
	"SUB",
	"MULT",
	"DIV",
	"SUM",
};

typedef enum TokenType
{
	TOKENTYPE_OPCODE,
	TOKENTYPE_LITERAL,
	TOKENTYPE_CELL,
} TokenType;

typedef struct Token
{
	TokenType type;
	union
	{
		double literal;
		Opcode opcode;
		Int2 pos;
	} value;
	struct Token* next;
} Token;

void initToken (Token* token)
{
	token->type = TOKENTYPE_OPCODE;
	token->value.opcode = OPCODE_EMPTY;
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
			case TOKENTYPE_CELL :
			{
				printf("CELL (%i, %i) -> ", token->value.pos.x, token->value.pos.y);
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

int tokenizeFormula (char* s, Token** token)
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
				
				int success = 0;
				for(int i = 0; i < OPCODECOUNT; i++)
				{
					if(stringEquals(build, opcodeSymbols[i]))
					{
						tail->type = TOKENTYPE_OPCODE;
						tail->value.opcode = (Opcode) i;
						success = 1;
						break;
					}
				}
				if(!success)
				{
					int result = sscanf(build, "%lf", &tail->value.literal);
					
					if(result == 1)
					{
						tail->type = TOKENTYPE_LITERAL;
						success = 1;
					}
				}
				if(!success)
				{
					char c;
					int n1, n2;
					int result = sscanf(build, "%c%d", &c, &n2);
					
					n1 = -1;
					for(int i = 0; i < ALPHABETSIZE; i++)
					{
						if(c == ALPHABET[i])
						{
							n1 = i;
							break;
						}
					}
					n2 = n2 - 1;
				
					if(result == 2 && n1 >= 0 && n2 >= 0)
					{
						tail->type = TOKENTYPE_CELL;
						tail->value.pos.x = n1;
						tail->value.pos.y = n2;
						success = 1;
					}
				}
				if(!success) { return 0; }
			}
			if(s[i] == '\0') { return 1; }
		}
		else
		{
			build[buildLength] = s[i];
			buildLength += 1;
		}
		i += 1;
	}
	
	return 1;
}