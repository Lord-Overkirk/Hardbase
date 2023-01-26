#ifndef _DBG_H
#define _DBG_H

#include <common.h>
#include "str.h"

typedef struct debug_command {
    int argc;
    char* command_type;
    uint32_t memory_start;
    uint32_t memory_end;
} debug_command;

debug_command parse_command(char*);
char* get_command(void);

enum debug_command_field {
    type = 1,
    start_addr = 2,
    end_addr = 3
};

#endif