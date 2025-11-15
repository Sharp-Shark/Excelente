#ifndef UTIL_H
#define UTIL_H

int intLength (int n);

int stringLength (const char* s);

int stringEquals (const char* s1, const char* s2);

void stringCopy (char* sTo, char* sFrom);

void printPadding (char c, int n);

#endif