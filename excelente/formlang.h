#ifndef FORMLANG_H
#define FORMLANG_H

#include "util.h"

typedef enum Opcode
{
	OPCODE_NOOP,
	OPCODE_INDEX,
	OPCODE_POS,
	OPCODE_ADD,
	OPCODE_SUB,
	OPCODE_MULT,
	OPCODE_DIV,
	OPCODECOUNT, // amount of opcodes defined
} Opcode;

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

void initToken (Token* token);

void freeToken (Token* token);

void printToken (Token* token);

int tokenizeFormula (char* s, Token** token);

#endif