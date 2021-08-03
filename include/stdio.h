#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdarg.h>

int printf(const char *, ...);
int sprintf(char *s, const char *format, ...);
int vsprintf(char *s, const char *format, va_list arg);

#endif
