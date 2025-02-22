#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stddef.h>
#include <wchar.h>

void srand(unsigned int seed);
int rand(void);
void exit(int status);
size_t wcstombs(char *dest, const wchar_t *src, size_t max);

#endif
