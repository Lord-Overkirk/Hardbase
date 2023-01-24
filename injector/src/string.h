#ifndef _STRING_H
#define _STRING_H

#include <common.h>

#define PRINTBUF 0x40e895e9
#define PRINTCRLF 0x40e88a49
#define STRLEN 0x40f146a5
#define SPRINTF 0x40f0c891

extern void (*printlen)(char *, int);
extern size_t (*strlen)(char*);
extern void (*printcrlf)(void);
extern int (*sprintf)(char*, char*, ...);

#endif // _STRING_H