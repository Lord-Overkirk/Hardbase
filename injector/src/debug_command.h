#ifndef _DBG_H
#define _DBG_H

#include <common.h>
#include "str.h"


typedef struct debug_command {
    int argc;
    char* command_type;
    char op;
    uint32_t memory_start;
    uint32_t memory_end;
} debug_command;

debug_command parse_command(char*);
char* get_command(void);


enum debug_command_field {
    TYPE = 1,
    OP = 2,
    START_ADDR = 3,
    END_ADDR = 4
};

#endif