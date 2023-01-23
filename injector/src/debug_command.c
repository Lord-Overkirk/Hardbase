#include <common.h>
#include "string.h"

// typedef struct debug_command {
//     uint32_t length;
//     uint32_t command_type;
//     uint32_t memory_start;
//     uint32_t memory_end;
// };

// void parse_command(char* input) {
//     uint32_t size = 0;
//     char* command;

//     size = get_command_size(input);
//     for (int i = 0; i < size; i++) {
//         if (input[i] == '=') {

//         }
//     }
// }



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