#ifndef _DBG_H
#define _DBG_H

#include <common.h>
#include "str.h"

typedef struct debug_command {
    char length[2];
    char command_type[3];
    uint32_t memory_start;
    uint32_t memory_end;
} debug_command;

debug_command parse_command(char*);
char* get_command(void);

enum command_type {
    memory = 0,
    registers = 1
};

#endif