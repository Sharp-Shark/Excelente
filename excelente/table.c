#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "collections.h"
#include "formlang.h"

#define MAXWIDTH 26
const char ALPHABET[MAXWIDTH + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0";

typedef enum CellType
{
	CELLTYPE_EMPTY,
	CELLTYPE_VALUE,
	CELLTYPE_FORMULA,
} CellType;

typedef struct Cell
{
	enum CellType type;
	double value;
	char* formula;
	Token* token;
	struct Cell* update;
} Cell;

void initCell (Cell* cell)
{
	cell->type = CELLTYPE_EMPTY;
	cell->value = 0.0;
	cell->formula = NULL;
}

void freeCell (Cell* cell)
{
	if(cell->formula != NULL)
	{
		free(cell->formula);
		cell->formula = NULL;
	}
	if(cell->token != NULL)
	{
		freeToken(cell->token);
		cell->token = NULL;
	}
	cell->type = CELLTYPE_EMPTY;
}

void printValueCell (Cell* cell, const unsigned int cellWidth)
{
	switch(cell->type)
	{
		case CELLTYPE_EMPTY :
		{
			printPadding('.', cellWidth);
			break;
		}
		case CELLTYPE_VALUE :
		case CELLTYPE_FORMULA :
		{
			char s[64];
			sprintf(s, "%lg", cell->value);
			s[cellWidth] = '\0';
			
			printPadding('.', cellWidth - stringLength(s));
			printf("%s", s);
			break;
		}
		default :
		{
			printPadding('X', cellWidth);
		}
	}
}

void printFormulaCell (Cell* cell)
{
	switch(cell->type)
	{
		case CELLTYPE_EMPTY :
		{
			printf("undefined\n");
			break;
		}
		case CELLTYPE_VALUE :
		{
			printf("%lf\n", cell->value);
			break;
		}
		case CELLTYPE_FORMULA :
		{
			printf("%s\n", cell->formula);
			break;
		}
		default :
		{
			printf("ERROR\n");
			break;
		}
	}
}

typedef struct Table
{
	unsigned int cellWidth;
	unsigned int width;
	unsigned int height;
	unsigned int area;
	Cell* cells;
} Table;

void initTable (Table* table, unsigned int width, unsigned int height)
{
	if(width > MAXWIDTH)
	{
		width = MAXWIDTH;
		printf("WARNING: Table width cannot exceed %d.\n", MAXWIDTH);
	}
	
	table->cellWidth = 8;
	table->width = width;
	table->height = height;
	table->area = width * height;
	table->cells = (Cell*) malloc(sizeof(Cell) * table->area);
	
	for(int i = 0; i < table->area; i++)
	{
		initCell(table->cells + i);
	}
}

void freeTable (Table* table)
{
	for(int i = 0; i < table->area; i++)
	{
		freeCell(table->cells + i);
	}
	free(table->cells);
}

Cell* indexTable (Table* table, unsigned int index)
{
	if(index >= table->area) { return table->cells + table->area - 1; }
	return table->cells + index;
}

Cell* posTable (Table* table, unsigned int x, unsigned int y)
{
	return indexTable(table, x + y * table->width);
}

void posToLabel (unsigned int x, unsigned int y, char* result)
{
	if(x > MAXWIDTH) { x = MAXWIDTH; }
	
	sprintf(result, "%c%d", ALPHABET[x], y + 1);
}

void calculateCellTable (Table* table, Cell* cell)
{
	if(cell->type != CELLTYPE_FORMULA) { return; }
	 
	// fixed memory for math
	DoubleStack stack;
	initDoubleStack(&stack, 8);
	Cell* cell1;
	double x1, x2;
	
	Token* token = cell->token;
	while(token != NULL)
	{
		if(token->type == TOKENTYPE_LITERAL)
		{
			appendDoubleStack(&stack, token->value.literal);
		}
		else
		{
			switch(token->value.opcode)
			{
				case OPCODE_INDEX :
				{
					popDoubleStack(&stack, &x1);
					cell1 = indexTable(table, (int) x1);
					calculateCellTable(table, cell1);
					appendDoubleStack(&stack, cell1->value);
					break;
				}
				case OPCODE_POS :
				{
					popDoubleStack(&stack, &x1);
					popDoubleStack(&stack, &x2);
					cell1 = posTable(table, (int) x2, (int) x1);
					calculateCellTable(table, cell1);
					appendDoubleStack(&stack, cell1->value);
					break;
				}
				case OPCODE_ADD :
				{
					popDoubleStack(&stack, &x1);
					popDoubleStack(&stack, &x2);
					appendDoubleStack(&stack, x1 + x2);
					break;
				}
				case OPCODE_MULT :
				{
					popDoubleStack(&stack, &x1);
					popDoubleStack(&stack, &x2);
					appendDoubleStack(&stack, x1 * x2);
					break;
				}
			}
		}
		token = token->next;
	}
	
	popDoubleStack(&stack, &cell->value);
	freeDoubleStack(&stack);
}

/*
	this function will later have something similar to the algorithm shown in class, although I'm not quite sure if I'd be able to create it 1:1
because the cells that a cell depends on is only known when the expression is evaluated. It might be beneficial to remove the AT (@) opcode
and, in its place, introduce an ADDRESS token type which points to a fixed cell determined at "parse-time" instead of at "evaluation-time"
	each time a cell has its value/formula modified, this function would be called on it and, in a branching manner, to all the cells whose
formula evaluation depends on the value of this one
	FOR NOW it currently just updates all cells which works just fine for now, even if not efficient
*/
void updateCellTable (Table* table, Cell* cell)
{
	for(int i = 0; i < table->area; i++)
	{
		calculateCellTable(table, table->cells + i);
	}
}

void setValueCellTable (Table* table, Cell* cell, double x)
{
	freeCell(cell);
	
	cell->type = CELLTYPE_VALUE;
	
	cell->value = x;
}

void setFormulaCellTable (Table* table, Cell* cell, char* s)
{
	freeCell(cell);
	
	cell->type = CELLTYPE_FORMULA;
	
	unsigned int size = stringLength(s);
	cell->formula = (char*) malloc(sizeof(char) * size);
	stringCopy(cell->formula, s);
	
	tokenizeFormula(s, &cell->token);
}

void printTable (Table* table, unsigned int x, unsigned int y)
{
	const unsigned int margin = intLength(table->height);
	
	int selected = 0;
	
	printPadding(' ', margin + 2);
	for(int i = 0; i < table->width; i++)
	{
		putchar(ALPHABET[i % MAXWIDTH]);
		printPadding(' ', table->cellWidth + 1);
	}
	putchar('\n');
	for(int i = 0; i < table->area; i++)
	{
		selected = x == i % table->width && y == i / table->width;
		
		if(i % table->width == 0)
		{
			if(i > 0) { putchar('\n'); }
			
			int n = i / table->width + 1;
			printPadding(' ', margin - intLength(n));
			printf("%d ", n);
		}
		if(selected)
		{
			putchar('>');
		}
		else
		{
			putchar(' ');
		}
		printValueCell(table->cells + i, table->cellWidth);
		if(selected)
		{
			putchar('<');
		}
		else
		{
			putchar(' ');
		}
	}
	putchar('\n');
}