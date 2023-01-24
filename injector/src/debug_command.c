#include "debug_command.h"
#include "str.h"


debug_command parse_command(char* input) {
    uint32_t size = strlen(input);
    char* command;

    for (uint32_t i = 0; i < size; i++) {
        if (input[i] == '=') {
            command = (char*)input + i;
        }
    }

    debug_command d;
    d.length[2] = '\0';
    d.command_type[3] = '\0';

    // Parse into the struct.
    for (uint32_t i = 0; i < strlen(command); i++) {
        if (command[i] >= '9') {
            d.command_type[i-3] = (char)command[i];
        } else {
            d.length[i-1] = (char)command[i];
        }
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