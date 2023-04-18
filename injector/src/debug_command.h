#ifndef _DBG_H
#define _DBG_H

#include <common.h>
#include "str.h"


typedef struct debug_command {
    int argc;
    char* command_type;
    char op;
    uint64_t memory_start;
    uint64_t memory_end;
    uint32_t payload_size;
    char payload[4];
} debug_command;

debug_command parse_command(char*);
char* get_command(void);


enum debug_command_field {
    TYPE = 1,
    OP = 2,
    START_ADDR = 3,
    END_ADDR = 4,
    PAYLOAD_SIZE = 5,
    PAYLOAD = 6
};

#endif