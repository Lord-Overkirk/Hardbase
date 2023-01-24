#ifndef _STRING_H
#define _STRING_H


// extern void (*printlen)(char*, int);
// extern unsigned int (*strlen)(char*);
// void (*printcrlf)(void) = (void*) 0x40e88a49;
// extern int (*sprintf)(char*, char*, ...);
// extern void pr();

extern void printlen(char*, int);
extern unsigned int strlen(char*);
// int sprintf(char* a, char* b, ...);
// int (*sprintf)(char*, char*, ...) ;
extern void printcrlf(void);

#endif // _STRING_H