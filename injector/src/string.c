#include "string.h"

void (*printlen)(char *, int) = (void*) PRINTBUF;
size_t (*strlen)(char*) = (void*) STRLEN;
void (*printcrlf)(void) = (void*) PRINTCRLF;
int (*sprintf)(char*, char*, ...) = (void*) SPRINTF;
