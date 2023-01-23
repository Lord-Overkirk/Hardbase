#include <common.h>

#ifndef _STRING_H
#define _STRING_H

void (*printlen)(char *, int);
size_t (*strlen)(char*);
void (*printcrlf)(void);
int (*sprintf)(char*, char*, ...);

#endif