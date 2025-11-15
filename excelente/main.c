#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "table.h"
#include "io.h"

/*

TO-DO: make modifying a table entry (cell) cause it to update cells that depend on it

*/

void tableTest ()
{
	struct Table table;
	initTable(&table, 8, 8);
	
	setValueCellTable(&table, posTable(&table, 0, 0), 0);
	setValueCellTable(&table, posTable(&table, 1, 0), 1);
	
	setFormulaCellTable(&table, posTable(&table, 2, 0), "0 @ 1 @ +\0");
	setFormulaCellTable(&table, posTable(&table, 3, 0), "1 @ 2 @ +\0");
	setFormulaCellTable(&table, posTable(&table, 4, 0), "2 @ 3 @ +\0");
	setFormulaCellTable(&table, posTable(&table, 5, 0), "3 @ 4 @ +\0");
	setFormulaCellTable(&table, posTable(&table, 6, 0), "4 @ 5 @ +\0");
	setFormulaCellTable(&table, posTable(&table, 7, 0), "5 @ 6 @ +\0");
	
	setValueCellTable(&table, posTable(&table, 0, 1), 0.5);
	setFormulaCellTable(&table, posTable(&table, 1, 1), "0 1 @@ 2 *\0");
	setFormulaCellTable(&table, posTable(&table, 2, 1), "1 1 @@ 2 *\0");
	setFormulaCellTable(&table, posTable(&table, 3, 1), "2 1 @@ 2 *\0");
	setFormulaCellTable(&table, posTable(&table, 4, 1), "3 1 @@ 2 *\0");
	setFormulaCellTable(&table, posTable(&table, 5, 1), "4 1 @@ 2 *\0");
	setFormulaCellTable(&table, posTable(&table, 6, 1), "5 1 @@ 2 *\0");
	setFormulaCellTable(&table, posTable(&table, 7, 1), "6 1 @@ 2 *\0");
	
	updateCellTable(&table, NULL);
	
	int x, y;
	x = 0;
	y = 0;
	
	int renderPending = 1;
	
	char c;
	while(1)
	{
		if(renderPending)
		{
			renderPending = 0;
			myclear();
			printTable(&table, x, y);
			printf("WASD to move\nQ to exit\n");
		}
		
		c = mygetch();
		switch(c)
		{
			case 's' :
			{
				y += 1;
				if(y > table.height - 1) { y = table.height - 1; }
				else { renderPending = 1; }
				break;
			}
			case 'w' :
			{
				y -= 1;
				if(y < 0) { y = 0; }
				else { renderPending = 1; }
				break;
			}
			case 'd' :
			{
				x += 1;
				if(x > table.width - 1) { x = table.width - 1; }
				else { renderPending = 1; }
				break;
			}
			case 'a' :
			{
				x -= 1;
				if(x < 0) { x = 0; }
				else { renderPending = 1; }
				break;
			}
			case 'q' :
			{
				return;
			}
		}
	}
	
	freeTable(&table);
}

int main ()
{
	tableTest();
	
	return 0;
}