#include "shannon.h"
#include <common.h>

// Address includes THUMB bit
#define PRINTBUF 0x40e895e9
#define PRINTCRLF 0x40e88a49
#define STRLEN 0x40f146a5
#define SPRINTF 0x40f0c891

void (*printlen)(char *, int) = (void*) PRINTBUF;
size_t (*strlen)(char*) = (void*) STRLEN;
void (*printcrlf)(void) = (void*) PRINTCRLF;
int (*sprintf)(char*, char*, ...) = (void*) SPRINTF;