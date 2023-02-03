#include "debug_command.h"
#include "str.h"
#include "assert.h"
#include "stdlib.h"


void print_debug_command(debug_command d) {
    char b[100];
    sprintf(b, "type: %s\r\nop: %c\r\nstart: %d\r\nend: %d\r\npayload_size %d\r\n",
            d.command_type, d.op, d.memory_start, d.memory_end, d.payload_size);
    printlen(b, strlen(b));
}

void payload_to_cmd(char* payload, debug_command* cmd) {
    char result[cmd->payload_size];
    char to_parse[2];
    char buff[50];
    for (size_t i = 0; i < cmd->payload_size; i++) {
        memcpy(to_parse, payload+(i*2), 2);
        sprintf(buff, "payload: %c\r\n", (char)strtol(to_parse, NULL, 16));
        printlen(buff, strlen(buff));
        result[i] = (char)strtol(to_parse, NULL, 16);
    }
    cmd->payload = result;
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

    char buff[50];
    char to_parse[2];
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
        case PAYLOAD_SIZE:
            d.payload_size = strtol(token, NULL, 16);
            break;
        case PAYLOAD:
            payload_to_cmd(token, &d);
            break;
        default:
            break;
        }
        token = strtok(NULL, "|");
    }
    // print_debug_command(d);
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