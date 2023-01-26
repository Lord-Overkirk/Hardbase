#include "assert.h"
#include "str.h"

void aFailed(char *file, int line) {
    char buffer[50];
    sprintf(buffer, "assertion failed in file %s on line %d\n", file, line);
    printlen(buffer, strlen(buffer));
}