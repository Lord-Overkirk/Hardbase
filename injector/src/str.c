#include "str.h"

#define PRINTBUF 0x40e895e9
#define PRINTCRLF 0x40e88a49
#define STRLEN 0x40f146a5
#define SPRINTF 0x40f0c891

void (*printlen_mem)(char*, int) = (void*) PRINTBUF;
unsigned int (*strlen_mem)(char*) = (void*) STRLEN;
void (*printcrlf_mem)(void) = (void*) PRINTCRLF;
// int (*sprintf)(char*, char*, ...) = (void*) SPRINTF;

void printlen(char* str, int length) {
    printlen_mem(str, length);
}

unsigned int strlen(char* str) {
    return strlen_mem(str);
}

void printcrlf() {
    printcrlf_mem();
}

// int sprintf(char* buff, char* fmt, ...) {
//     __va_list_tag
//     sprintf_mem(buff, fmt);
// }
