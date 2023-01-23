#include <common.h>

void (*printlen)(char *, int);
size_t (*strlen)(char*);
void (*printcrlf)(void);
int (*sprintf)(char*, char*, ...);