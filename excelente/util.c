#include <stdio.h>

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

void stringCopy (char* sTo, char* sFrom)
{
	int i = 0;
	while(sFrom[i] != '\0')
	{
		sTo[i] = sFrom[i];
		i += 1;
	}
	sTo[i] = '\0';
}

void printPadding (char c, int n)
{
	for(int i = 0; i < n; i++)
	{
		putchar(c);
	}
}