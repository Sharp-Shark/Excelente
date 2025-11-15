#ifndef COLLECTIONS_H
#define COLLECTIONS_H

typedef struct DoubleStack
{
	double* ptr;
	unsigned int len;
	unsigned int size;
} DoubleStack;

void initDoubleStack (DoubleStack* stack, int size);

void reallocDoubleStack (DoubleStack* stack, int size);

void appendDoubleStack (DoubleStack* stack, double x);

void popDoubleStack (DoubleStack* stack, double* result);

void printDoubleStack (DoubleStack* stack);

void freeDoubleStack (DoubleStack* stack);

typedef struct IntStack
{
	int* ptr;
	unsigned int len;
	unsigned int size;
} IntStack;

void initIntStack (IntStack* stack, int size);

void reallocIntStack (IntStack* stack, int size);

void appendIntStack (IntStack* stack, int x);

void popIntStack (IntStack* stack, int* result);

void getIndexIntStack (IntStack* stack, int target, int* result);

void printIntStack (IntStack* stack);

void freeIntStack (IntStack* stack);

#endif