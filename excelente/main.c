#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "table.h"
#include "io.h"
#include "formlang.h"

extern const unsigned int ALPHABETSIZE;
extern const char ALPHABET[];

void tableTest ()
{	
	struct Table table;
	initTable(&table, 16, 16);
	
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
	
	setValueCellTable(&table, posTable(&table, 0, 2), 3.141592653589793);
	setFormulaCellTable(&table, posTable(&table, 1, 2), "1 A3 /\0");
	
	updateCellTable(&table, NULL);
	
	char message[256] = "";
	char label[8];
	
	int x, y;
	x = 0;
	y = 0;
	
	int printExcelente = 1;
	
	int renderPending = 1;
	
	char c;
	while(1)
	{
		if(renderPending)
		{
			renderPending = 0;
			myclear();
			
			if(printExcelente)
			{
				printf("\
█████ █   █  ███   ███  █     ███  █   █ ███████  ███ \n\
█      █ █  █   █ █   █ █    █   █ ██  █    █    █   █\n\
█████   █   █     █████ █    █████ █ █ █    █    █████\n\
█      █ █  █   █ █     █    █     █  ██    █    █    \n\
█████ █   █  ███   ████ ████  ████ █   █    █     ████\n\n");
				printExcelente = 0;
			}
			
			printf("%s", message);
			stringCopy(message, "\0");
			
			printTable(&table, x, y);
			
			posToLabel(x, y, label);
			printf("%s = ", label);
			printFormulaCell(posTable(&table, x, y));
			printf(" (= %lf)\n\n", posTable(&table, x, y)->value);
			
			printf("WASD to move\nE to modify cell\nZX to change cell width\nC to resize table\nQ to quit\n");
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
			case 'z' :
			{
				table.cellWidth -= 1;
				if(table.cellWidth < 1) { table.cellWidth = 1; }
				else { renderPending = 1; }
				break;
			}
			case 'x' :
			{
				table.cellWidth += 1;
				if(table.cellWidth > 32) { table.cellWidth = 32; }
				else { renderPending = 1; }
				break;
			}
			case 'c' :
			{
				int result;
				int width, height;
				
				printf("\nINSERT WIDTH\n");
				result = scanf("%d", &width);
				if(width < 1) { width = 1; }
				if(width > ALPHABETSIZE) { width = ALPHABETSIZE; }
				if(result == 0)
				{
					stringCopy(message, "[!] INVALID WIDTH\n\0");
					break;
				}
				
				printf("\nINSERT HEIGHT\n");
				result = scanf("%d", &height);
				if(height < 1) { height = 1; }
				if(height > ALPHABETSIZE) { height = ALPHABETSIZE; }
				if(result == 0)
				{
					stringCopy(message, "[!] INVALID HEIGHT\n\0");
					break;
				}
				
				resizeTable(&table, width, height);
				
				x = 0;
				y = 0;
				
				renderPending = 1;
				break;
			}
			case 'e' :
			{
				char s[128];
				printf("\nINSERT VALUE\n");
				fgets(s, 128, stdin);
				
				Cell* cell = posTable(&table, x, y);
				int result = setCellTable(&table, cell, s);
				if(result > 0)
				{
					updateCellTable(&table, NULL);
					
					if(result == 2)
					{
						posToLabel(x, y, label);
						
						sprintf(message, "[!] EMPTIED CELL %s\n", label);
					}
				}
				else
				{
					posToLabel(x, y, label);
					
					int i = 0;
					int j = 0;
					while(s[i] != '\0')
					{
						if(s[i] != '\n')
						{
							s[j] = s[i];
							j += 1;
						}
						i += 1;
					}
					s[j] = '\0';
					
					sprintf(message, "[!] ATTEMPT TO INSERT INVALID FORMULA \"%s\" INTO CELL %s\n", s, label);
				}
				
				renderPending = 1;
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