#include "debug_command.h"
#include "str.h"
#include "assert.h"
#include "stdlib.h"


void print_debug_command(debug_command d) {
    char b[100];
    sprintf(b, "type: %s\r\nop: %c\r\nstart: %d\r\nend: %d\r\n", d.command_type, d.op, d.memory_start, d.memory_end);
    printlen(b, strlen(b));
}

/**
 * @brief Parse the passed command into a defined struct.
 * 
 * @param input the raw command from the AT modem serial
 * @return debug_command, the parsed command.
 */
debug_command parse_command(char* input) {
    /* Find start of command. */
    char* command = strtok(input, "=");
    char* res = "";
    while (strcmp(command, "#")) {
        res = command;
        command = strtok(NULL, "=");
    }

    debug_command d;
    d.argc = 0;
    char* token = strtok(res, "|");
    while (strcmp(token, "#")) {
        d.argc++;
        switch (d.argc) {
        case TYPE:
            d.command_type = token;
            break;
        case OP:
            d.op = token[0];
            break;
        case START_ADDR:
            d.memory_start = strtol(token, NULL, 16);
            break;
        case END_ADDR:
            d.memory_end = strtol(token, NULL, 16);
            break;
        default:
            break;
        }
        token = strtok(NULL, "|");
    }
    return d;
}



/**
 * @brief Locates the argument passed to AT in the baseband memory.
 * The exact location has been determined through reverse engineering the firmware in ghidra
 * 
 * @return char*. The command passed after the 'AT' command.
 */
char* get_command() {
    uint32_t mem = *(uint32_t*)(uint32_t)*(uint32_t*)0x40aad9c4;
    uint32_t* mem2 = (uint32_t*)(mem + 0x1b8);
    char* str = (char*)(mem2);
    return (char*)(*(int*)str);
}