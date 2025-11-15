#ifndef FORMLANG_H
#define FORMLANG_H

typedef enum Opcode
{
	OPCODE_NOOP,
	OPCODE_INDEX,
	OPCODE_POS,
	OPCODE_ADD,
	OPCODE_MULT,
	OPCODECOUNT, // amount of opcodes defined
} Opcode;

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

void initToken (Token* token);

void freeToken (Token* token);

void printToken (Token* token);

void tokenizeFormula (char* s, Token** token);

#endif