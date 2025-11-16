#ifndef TABLE_H
#define TABLE_H

#include "formlang.h"

typedef enum CellType
{
	CELLTYPE_EMPTY,
	CELLTYPE_VALUE,
	CELLTYPE_FORMULA,
	CELLTYPE_FORMULAINVALID,
} CellType;

typedef struct Cell
{
	enum CellType type;
	double value;
	char* formula;
	Token* token;
	struct Cell* update;
} Cell;

void initCell (Cell* cell);

void freeCell (Cell* cell);

void printValueCell (Cell* cell, const unsigned int cellWidth);

void printFormulaCell (Cell* cell);

typedef struct Table
{
	unsigned int cellWidth;
	unsigned int width;
	unsigned int height;
	unsigned int area;
	Cell* cells;
} Table;

void initTable (Table* table, unsigned int width, unsigned int height);

void resizeTable (Table* table, unsigned int width, unsigned int height);

void freeTable (Table* table);

int isInvalidCell (Cell* cell);

Cell* indexTable (Table* table, unsigned int index);

Cell* posTable (Table* table, unsigned int x, unsigned int y);

void posToLabel (unsigned int x, unsigned int y, char* s);

void calculateCellTable (Table* table, Cell* cell);

void updateCellTable (Table* table, Cell* cell);

int setValueCellTable (Table* table, Cell* cell, double x);

int setFormulaCellTable (Table* table, Cell* cell, char* s);

int setTextCellTable (Table* table, Cell* cell, char* s);

int setCellTable (Table* table, Cell* cell, char* s);

void printTable (Table* table, unsigned int x, unsigned int y);

#endif