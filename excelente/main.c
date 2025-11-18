#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "table.h"
#include "io.h"
#include "formlang.h"

#define SAVEFILENAME "save.excel"

extern const unsigned int ALPHABETSIZE;
extern const char ALPHABET[];

void tableTest ()
{	
	struct Table table;
	initTable(&table, 8, 8);
	table.cellWidth = 10;
	
	setTextCellTable(&table, posTable(&table, 0, 0), "Sales\0");
	setValueCellTable(&table, posTable(&table, 1, 0), 2);
	setValueCellTable(&table, posTable(&table, 2, 0), 5);
	setValueCellTable(&table, posTable(&table, 3, 0), 3);
	setValueCellTable(&table, posTable(&table, 4, 0), 8);
	setValueCellTable(&table, posTable(&table, 5, 0), 4);
	setValueCellTable(&table, posTable(&table, 6, 0), 2);
	setValueCellTable(&table, posTable(&table, 7, 0), 1);
	
	setTextCellTable(&table, posTable(&table, 0, 1), "Total\0");
	setFormulaCellTable(&table, posTable(&table, 1, 1), "B1 C1 D1 E1 F1 G1 H1 SUM\0");
	
	setTextCellTable(&table, posTable(&table, 0, 2), "Fibonacci\0");
	setValueCellTable(&table, posTable(&table, 1, 2), 1);
	setValueCellTable(&table, posTable(&table, 2, 2), 1);
	setFormulaCellTable(&table, posTable(&table, 3, 2), "B3 C3 +");
	setFormulaCellTable(&table, posTable(&table, 4, 2), "C3 D3 +");
	setFormulaCellTable(&table, posTable(&table, 5, 2), "D3 E3 +");
	setFormulaCellTable(&table, posTable(&table, 6, 2), "E3 F3 +");
	setFormulaCellTable(&table, posTable(&table, 7, 2), "F3 G3 +");
	
	/* if the program lags whilst running this table it is because I haven't added optimization yet
	setValueCellTable(&table, indexTable(&table, 0), 1);
	setValueCellTable(&table, indexTable(&table, 1), 1);
	char formula[64];
	for(int i = 2; i < table.area; i++)
	{
		sprintf(formula, "%d @ %d @ +", i - 2, i - 1);
		setFormulaCellTable(&table, indexTable(&table, i), formula);
	}
	*/
	
	updateCellTable(&table, NULL);
	
	char message[256] = "";
	char label[8];
	
	int x, y;
	x = 0;
	y = 0;
	
	const unsigned int bufferSize = 4096;
	char* buffer = (char*) malloc(sizeof(char) * (bufferSize + 1));
	setvbuf(stdout, buffer, _IOFBF, bufferSize);
	
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
			
			Cell* cell = posTable(&table, x, y);
			if(cell != NULL)
			{
				posToLabel(x, y, label);
				printf("%s = ", label);
				printFormulaCell(cell);
				if(cell->type == CELLTYPE_FORMULA)
				{
					printf(" (= %lf)\n\n", cell->value);
				}
				else
				{
					printf("\n\n");
				}
			}
			
			printf("WASD to move\nE to modify cell\nZX to change cell width\nC to resize table\nF to save to file\nG to load from file\nQ to quit\n");
			
			fflush(stdout);
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
				fflush(stdout);
				result = scanf("%d", &width);
				if(width < 1) { width = 1; }
				if(width > ALPHABETSIZE) { width = ALPHABETSIZE; }
				if(result == 0)
				{
					stringCopy(message, "[!] INVALID WIDTH\n\0");
					break;
				}
				
				printf("\nINSERT HEIGHT\n");
				fflush(stdout);
				result = scanf("%d", &height);
				if(height < 1) { height = 1; }
				if(height > ALPHABETSIZE) { height = ALPHABETSIZE; }
				if(result == 0)
				{
					stringCopy(message, "[!] INVALID HEIGHT\n\0");
					break;
				}
				
				resizeTable(&table, width, height);
				
				if(x > width - 1) { x = width - 1; }
				if(y > height - 1) { y = height - 1; }
				
				sprintf(message, "[!] RESIZED TABLE TO %d BY %d\n", width, height);
				
				renderPending = 1;
				break;
			}
			case 'e' :
			{
				char s[128];
				printf("\nINSERT VALUE\n");
				fflush(stdout);
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
			case 'f' :
			{
				FILE* file = fopen(SAVEFILENAME, "w");
				saveToFileTable(&table, file);
				fclose(file);
				
				sprintf(message, "[!] SAVED TABLE TO FILE \"%s\"\n", SAVEFILENAME);
				
				renderPending = 1;
				break;
			}
			case 'g' :
			{
				FILE* file = fopen(SAVEFILENAME, "r");
				
				if(file != NULL)
				{
					loadFromFileTable(&table, file);
					fclose(file);
					
					if(x > table.width - 1) { x = table.width - 1; }
					if(y > table.height - 1) { y = table.height - 1; }
					
					sprintf(message, "[!] LOADED TABLE FROM FILE \"%s\"\n", SAVEFILENAME);
				}
				else
				{
					sprintf(message, "[!] FILE \"%s\" NOT FOUND\n", SAVEFILENAME);
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
	
	free(buffer);
	freeTable(&table);
}

int main ()
{
	tableTest();
	
	return 0;
}