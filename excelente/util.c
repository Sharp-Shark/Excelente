#include <stdio.h>

typedef struct Int2
{
	unsigned int x;
	unsigned int y;
} Int2;

int intLength (int n)
{
	int len = n < 0;
	while(n > 0)
	{
		n /= 10;
		len += 1;
	}
	return len;
}

int stringLength (const char* s)
{
	int i = 0;
	while(1)
	{
		if(s[i] == '\0') { return i; }
		i += 1;
	}
}

int stringEquals (const char* s1, const char* s2)
{
	int i = 0;
	while(s1[i] != '\0' && s2[i] != '\0')
	{
		if(s1[i] != s2[i]) { return 0; }
		i += 1;
	}
	if(s1[i] != s2[i]) { return 0; }
	return 1;
}

void stringCopy (char* sTo, const char* sFrom)
{
	int i = 0;
	while(sFrom[i] != '\0')
	{
		sTo[i] = sFrom[i];
		i += 1;
	}
	sTo[i] = '\0';
}

void stringSanitize (char* s, int skip)
{
	int i = 0;
	int j = 0;
	int write = 0;
	while(s[i] != '\0')
	{
		if(i > skip && s[i] != ' ')
		{
			write = 1;
		}
		if(write && s[i] != '\n') {
			s[j] = s[i];
			j += 1;
		}
		i += 1;
	}
	s[j] = '\0';
}

void printPadding (char c, int n)
{
	for(int i = 0; i < n; i++)
	{
		putchar(c);
	}
}