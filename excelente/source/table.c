#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "collections.h"
#include "formlang.h"

#define MAXWIDTH 26
const unsigned int ALPHABETSIZE = MAXWIDTH;
const char ALPHABET[MAXWIDTH + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0";

typedef enum CellType
{
	CELLTYPE_EMPTY,
	CELLTYPE_VALUE,
	CELLTYPE_FORMULA,
	CELLTYPE_FORMULAINVALID,
	CELLTYPE_TEXT,
} CellType;

typedef struct Cell
{
	enum CellType type;
	double value;
	char* formula;
	Token* token;
	IntStack update;
} Cell;

void initCell (Cell* cell)
{
	cell->type = CELLTYPE_EMPTY;
	cell->value = 0.0;
	cell->formula = NULL;
	cell->token = NULL;
}

void freeCell (Cell* cell)
{
	cell->type = CELLTYPE_EMPTY;
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
	cell->value = 0.0;
}

int isInvalidCell (Cell* cell)
{
	switch(cell->type)
	{
		case CELLTYPE_VALUE :
		case CELLTYPE_FORMULA :
			return 0;
		default :
			return 1;
	}
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
			char* s = (char*) malloc(sizeof(char) * (cellWidth + 1));
			sprintf(s, "%lg", cell->value);
			s[cellWidth] = '\0';
			
			printPadding('.', cellWidth - stringLength(s));
			printf("%s", s);
			
			free(s);
			break;
		}
		case CELLTYPE_FORMULAINVALID :
		{
			printPadding('?', cellWidth);
			break;
		}
		case CELLTYPE_TEXT :
		{
			char* s = (char*) malloc(sizeof(char) * (cellWidth + 1));
			int length = 0;
			while(length < cellWidth && cell->formula[length] != '\0')
			{
				s[length] = cell->formula[length];
				length += 1;
			}
			s[length] = '\0';
			
			printPadding('.', cellWidth - length);
			printf("%s", s);
			
			free(s);
			break;
		}
		default :
		{
			printPadding('!', cellWidth);
		}
	}
}

void printFormulaCell (Cell* cell)
{
	switch(cell->type)
	{
		case CELLTYPE_EMPTY :
		{
			printf("empty");
			break;
		}
		case CELLTYPE_VALUE :
		{
			printf("%lf", cell->value);
			break;
		}
		case CELLTYPE_FORMULA :
		case CELLTYPE_FORMULAINVALID :
		{
			printf("%s", cell->formula);
			break;
		}
		case CELLTYPE_TEXT :
		{
			printf("\"%s\"", cell->formula);
			break;
		}
		default :
		{
			printf("ERROR");
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

void resizeTable (Table* table, unsigned int width, unsigned int height)
{
	if(width > MAXWIDTH)
	{
		width = MAXWIDTH;
	}
	
	table->width = width;
	table->height = height;
	table->area = width * height;
	table->cells = (Cell*) realloc(table->cells, sizeof(Cell) * table->area);
	
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
	if(index < 0) { return table->cells; }
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

/*
	I should later improve the circular dependency detection on this. Currently it can only detect direct circular dependency
	e.g.: it will catch "A1 = A1" but it won't catch "A1 = B2", "B2 = A1"
*/
void calculateCellTable (Table* table, Cell* cell)
{
	if(cell->type != CELLTYPE_FORMULA && cell->type != CELLTYPE_FORMULAINVALID) { return; }
	cell->type = CELLTYPE_FORMULA;
	
	// fixed memory for math
	DoubleStack stack;
	initDoubleStack(&stack, 8);
	Cell* cell1;
	double x1, x2;
	
	Token* token = cell->token;
	while(token != NULL)
	{
		switch(token->type)
		{
			case TOKENTYPE_LITERAL :
			{
				appendDoubleStack(&stack, token->value.literal);
				break;
			}
			case TOKENTYPE_CELL :
			{
				cell1 = posTable(table, token->value.pos.x, token->value.pos.y);
				if(cell != cell1) { calculateCellTable(table, cell1); }
				if(cell == cell1 || isInvalidCell(cell1))
				{
					cell->type = CELLTYPE_FORMULAINVALID;
				}
				appendDoubleStack(&stack, cell1->value);
				break;
			}
			case TOKENTYPE_OPCODE :
			{
				switch(token->value.opcode)
				{
					case OPCODE_INDEX :
					{
						popDoubleStack(&stack, &x1);
						cell1 = indexTable(table, (int) x1);
						if(cell != cell1) { calculateCellTable(table, cell1); }
						if(cell == cell1 || isInvalidCell(cell1))
						{
							cell->type = CELLTYPE_FORMULAINVALID;
						}
						appendDoubleStack(&stack, cell1->value);
						break;
					}
					case OPCODE_POS :
					{
						popDoubleStack(&stack, &x2);
						popDoubleStack(&stack, &x1);
						cell1 = posTable(table, (int) x1, (int) x2);
						if(cell != cell1) { calculateCellTable(table, cell1); }
						if(cell == cell1 || isInvalidCell(cell1))
						{
							cell->type = CELLTYPE_FORMULAINVALID;
						}
						appendDoubleStack(&stack, cell1->value);
						break;
					}
					case OPCODE_ADD :
					{
						popDoubleStack(&stack, &x2);
						popDoubleStack(&stack, &x1);
						appendDoubleStack(&stack, x1 + x2);
						break;
					}
					case OPCODE_SUB :
					{
						popDoubleStack(&stack, &x2);
						popDoubleStack(&stack, &x1);
						appendDoubleStack(&stack, x1 - x2);
						break;
					}
					case OPCODE_MULT :
					{
						popDoubleStack(&stack, &x2);
						popDoubleStack(&stack, &x1);
						appendDoubleStack(&stack, x1 * x2);
						break;
					}
					case OPCODE_DIV :
					{
						popDoubleStack(&stack, &x2);
						popDoubleStack(&stack, &x1);
						appendDoubleStack(&stack, x1 / x2);
						break;
					}
					case OPCODE_SUM :
					{
						x1 = 0.0;
						while(stack.len > 0)
						{
							popDoubleStack(&stack, &x2);
							x1 += x2;
						}
						appendDoubleStack(&stack, x1);
						break;
					}
					default :
					{
						cell->type = CELLTYPE_FORMULAINVALID;
					}
				}
				break;
			}
			default :
			{
				cell->type = CELLTYPE_FORMULAINVALID;
			}
		}
		token = token->next;
	}
	
	popDoubleStack(&stack, &cell->value);
	freeDoubleStack(&stack);
}

void updateCellTable (Table* table, Cell* cell)
{
	if(cell == NULL)
	{
		for(int i = 0; i < table->area; i++)
		{
			calculateCellTable(table, table->cells + i);
		}
	}
	else
	{
		/*
			TO-DO: if a cell is specified, instead of in an expensive manner updating all cells, only update the cell passed
			and the cells whose formula's calculation depends on it (and so on in an recursive manner)
		*/
		calculateCellTable(table, cell);
	}
}

int setValueCellTable (Table* table, Cell* cell, double x)
{
	freeCell(cell);
	
	cell->type = CELLTYPE_VALUE;
	
	cell->value = x;
	
	return 1;
}

int setFormulaCellTable (Table* table, Cell* cell, char* s)
{
	freeCell(cell);
	
	cell->type = CELLTYPE_FORMULA;
	
	unsigned int size = stringLength(s) + 1;
	cell->formula = (char*) malloc(sizeof(char) * size);
	stringCopy(cell->formula, s);
	
	int result = tokenizeFormula(s, &cell->token);
	if(result == 0)
	{
		freeCell(cell);
		return result;
	}
	
	if(cell->token == NULL)
	{
		freeCell(cell);
		return 2;
	}
	else if(cell->token->type == TOKENTYPE_LITERAL && cell->token->next == NULL)
	{
		return setValueCellTable(table, cell, cell->token->value.literal);
	}
	
	return 1;
}

int setTextCellTable (Table* table, Cell* cell, char* s)
{
	freeCell(cell);
	
	cell->type = CELLTYPE_TEXT;
	
	unsigned int size = stringLength(s) + 1;
	cell->formula = (char*) malloc(sizeof(char) * size);
	stringCopy(cell->formula, s);
	
	return 1;
}

int setCellTable (Table* table, Cell* cell, char* s)
{
	if(s == NULL || s[0] == '\n' || s[0] == '\0')
	{
		freeCell(cell);
		
		return 2;
	}
	else if(s[0] == '=')
	{
		stringSanitize(s, 0);
		
		return setFormulaCellTable(table, cell, s);
	}
	else if(s[0] == '"')
	{
		stringSanitize(s, 0);
		
		return setTextCellTable(table, cell, s);
	}
	else
	{
		double x;
		int result = sscanf(s, "%lf", &x);
		if(result > 0)
		{
			return setValueCellTable(table, cell, x);
		}
		else
		{
			stringSanitize(s, -1);
			
			return setTextCellTable(table, cell, s);
		}
	}
	return 0;
}

void printTable (Table* table, unsigned int x, unsigned int y)
{
	const unsigned int margin = intLength(table->height);
	
	int selected = 0;
	
	printPadding(' ', margin + 3);
	for(int i = 0; i < table->width; i++)
	{
		selected = x == i;
		if(selected)
		{
			putchar('>');
		}
		else
		{
			putchar(' ');
		}
		printPadding(ALPHABET[i % MAXWIDTH], table->cellWidth);
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
	for(int i = 0; i < table->area; i++)
	{
		if(i % table->width == 0)
		{
			if(i > 0) { putchar('\n'); }
			
			selected = y == i / table->width;
			if(selected)
			{
				putchar('>');
			}
			else
			{
				putchar(' ');
			}
			int n = i / table->width + 1;
			printPadding('0', margin - intLength(n));
			printf("%d", n);
			if(selected)
			{
				putchar('<');
			}
			else
			{
				putchar(' ');
			}
			putchar(' ');
		}
		
		selected = x == i % table->width && y == i / table->width;
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

void saveToFileTable (Table* table, FILE* file)
{
	fwrite(&table->width, sizeof(int), 1, file);
	fwrite(&table->height, sizeof(int), 1, file);
	
	Cell* cell;
	for(int i = 0; i < table->area; i++)
	{
		cell = table->cells + i;
		
		fwrite(&cell->type, sizeof(CellType), 1, file);
		switch(cell->type)
		{
			case CELLTYPE_EMPTY :
			{
				break;
			}
			case CELLTYPE_VALUE :
			{
				fwrite(&cell->value, sizeof(double), 1, file);
				
				break;
			}
			case CELLTYPE_FORMULA :
			case CELLTYPE_FORMULAINVALID :
			case CELLTYPE_TEXT :
			{
				unsigned int size = stringLength(cell->formula) + 1;
				fwrite(&size, sizeof(int), 1, file);
				fwrite(cell->formula, sizeof(char), size, file);
				
				break;
			}
		}
	}
}

void loadFromFileTable (Table* table, FILE* file)
{
	unsigned int width, height;
	fread(&width, sizeof(int), 1, file);
	fread(&height, sizeof(int), 1, file);
	
	resizeTable(table, width, height);
	
	Cell* cell;
	for(int i = 0; i < table->area; i++)
	{
		cell = table->cells + i;
		
		fread(&cell->type, sizeof(CellType), 1, file);
		switch(cell->type)
		{
			case CELLTYPE_EMPTY :
			{
				break;
			}
			case CELLTYPE_VALUE :
			{
				double x;
				fread(&x, sizeof(double), 1, file);
				
				setValueCellTable(table, cell, x);
				
				break;
			}
			case CELLTYPE_FORMULA :
			case CELLTYPE_FORMULAINVALID :
			{
				unsigned int size;
				fread(&size, sizeof(int), 1, file);
				char* s;
				s = (char*) malloc(sizeof(char) * size);
				fread(s, sizeof(char), size, file);
				
				setFormulaCellTable(table, cell, s);
				
				break;
			}
			case CELLTYPE_TEXT :
			{
				unsigned int size;
				fread(&size, sizeof(int), 1, file);
				char* s;
				s = (char*) malloc(sizeof(char) * size);
				fread(s, sizeof(char), size, file);
				
				setTextCellTable(table, cell, s);
				
				break;
			}
		}
	}
	
	updateCellTable(table, NULL);
}