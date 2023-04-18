#ifndef _STRING_H
#define _STRING_H

#define PRINTBUF 0x40e895e9
#define PRINTCRLF 0x40e88a49
#define STRLEN 0x40f146a5
#define SPRINTF 0x40f0c891
#define STRTOK 0x40f7c62d
#define STRTOL 0x40a9794d
#define STRTOUL 0x40a8b009

#define printlen ((void (*)(char*, int))PRINTBUF)
#define strlen ((unsigned int (*)(char*))STRLEN)
#define printcrlf ((void (*)(void))PRINTCRLF)
#define sprintf ((int (*)(char*, char*, ...))SPRINTF)
#define strtok ((char* (*)(char*, const char*))STRTOK)
#define strtol ((long (*)(char*, char**, int))STRTOL)
#define strtoul ((long (*)(char*, char**, int))STRTOUL)


#endif // _STRING_H