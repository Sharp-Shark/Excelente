#include <stdio.h>
#include <stdlib.h>
#include "util.h"

/*
	I should later make a collection whose members are an union so I don't have to make a DoubleStack, CharStack, IntStack, IntPointerStack ad infinitum
*/

typedef struct DoubleStack
{
	double* ptr;
	unsigned int len;
	unsigned int size;
} DoubleStack;

void initDoubleStack (DoubleStack* stack, int size)
{
	stack->ptr = (double*) malloc(size * sizeof(double));
	stack->len = 0;
	stack->size = size;
}

void reallocDoubleStack (DoubleStack* stack, int size)
{
	stack->ptr = (double*) realloc(stack->ptr, size * sizeof(double));
	stack->size = size;
}

void appendDoubleStack (DoubleStack* stack, double x)
{
	if (stack->len >= stack->size)
	{
		reallocDoubleStack(stack, 2 * stack->size);
	}
	stack->ptr[stack->len] = x;
	stack->len += 1;
}

void popDoubleStack (DoubleStack* stack, double* result)
{
	if (stack->len <= 0) { return; }
	if (result != NULL)
	{
		*result = stack->ptr[stack->len - 1];
	}
	stack->len -= 1;
}

void printDoubleStack (DoubleStack* stack)
{
	for (int i = 0; i < stack->len; i++)
	{
		printf("%d: %lf\n", i, stack->ptr[i]);
	}
}

void freeDoubleStack (DoubleStack* stack)
{
	free(stack->ptr);
}

typedef struct IntStack
{
	int* ptr;
	unsigned int len;
	unsigned int size;
} IntStack;

void initIntStack (IntStack* stack, int size)
{
	stack->ptr = (int*) malloc(size * sizeof(int));
	stack->len = 0;
	stack->size = size;
}

void reallocIntStack (IntStack* stack, int size)
{
	stack->ptr = (int*) realloc(stack->ptr, size * sizeof(int));
	stack->size = size;
}

void appendIntStack (IntStack* stack, int x)
{
	if (stack->len >= stack->size)
	{
		reallocIntStack(stack, 2 * stack->size);
	}
	stack->ptr[stack->len] = x;
	stack->len += 1;
}

void popIntStack (IntStack* stack, int* result)
{
	if (stack->len <= 0) { return; }
	if (result != NULL)
	{
		*result = stack->ptr[stack->len - 1];
	}
	stack->len -= 1;
}

int getIndexIntStack (IntStack* stack, int target)
{
	for(int i = 0; i < stack->len; i++)
	{
		if(stack->ptr[i] == target)
		{
			return i;
		}
	}
	return -1;
}

void printIntStack (IntStack* stack)
{
	for (int i = 0; i < stack->len; i++)
	{
		printf("%d: %d\n", i, stack->ptr[i]);
	}
}

void printNamedIntStack (IntStack* stack, const char** names)
{
	for (int i = 0; i < stack->len; i++)
	{
		printf("%d: %s\n", i, names[stack->ptr[i]]);
	}
}

void freeIntStack (IntStack* stack)
{
	free(stack->ptr);
}