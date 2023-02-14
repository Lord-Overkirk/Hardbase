#include "str.h"
#include "shannon.h"
#include "registers.h"
#include "assert.h"
#include "debug_command.h"
#include "stdlib.h"
#include <common.h>

#define PREFETCH_ABORT 0x400100bc

const char TASK_NAME[] = "DEBUG\0";

int (*ch_select)(void) = (void*) 0x40aad8d9;


/* Print len bytes as hexadecimals from specified addr. */
void print_hex(char* in, int len) {
    for (int i = 0; i < len; i++) {
        char b = in[i];
        char h = (b >> 4) & 0xf;
        char l = b & 0xf;

        char r1 = (h >= 10 ? 'a' + (h-10) : '0' + h);
        char r2 = (l >= 10 ? 'a' + (l-10) : '0' + l);
        char s1[1] = {r1};
        char s2[1] = {r2};

        printlen(s1, 1);
        printlen(s2, 1);
        printlen(" ", 1);
    }
}

static inline void write_memory(unsigned int start, char* payload, unsigned int size) {
    volatile char* dst = (char*) start;
    // for (unsigned int i = 0; i < size; i++) {
    //     *(mem) = 0x70;
    //     *(mem+1) = 0x70;
    // }
    memcpy((void*)dst, payload, size);
}

/* Dump the bytes as hex in the specified range. */
void dump_byte_range(unsigned int start, unsigned int end) {
    if (start > end) {
        char* err_str = "Error, wrong range\n";
        printlen(err_str, strlen(err_str));
        return;
    }
    // ASSERT(start > end);
    print_hex((char*) start, end-start);
}

// void debugger_hook() {
//     static int beenhere = 0;

//     if (beenhere != 2){
//         char* a = "YOHAY1";
//         size_t y_len = strlen(a);

//         char buffer[50];
//         int d = 10, b = 20, c;
//         c = d + b;
//         sprintf(buffer, "Sum of %d and %d is %d", d, b, task_count());
//         printlen(buffer, strlen(buffer));

//         printcrlf();
//         print_hex(a, 7);
//         printcrlf();

//         dump_byte_range(0x40669586, 0x4066959f);
//         printcrlf();
//         dump_byte_range(0x4066959f, 0x40669586);
//         printcrlf();
//         // print_hex(get_pc(), 4);
//         char buffer2[50];
//         sprintf(buffer2, "The pc is: %x", get_pc());
//         printlen(buffer2, strlen(buffer2));

//         char buffer3[50];
//         sprintf(buffer3, "The r25 is: %x", get_r3());
//         printlen(buffer3, strlen(buffer3));
//         printcrlf();

//         beenhere++;
//     }
//     return;
// }

/**
 * @brief Send all the register contents to the gdbserver.
 * 
 */
static inline void print_regs() {
    char buffer[50];
    sprintf(buffer, "r0: 0x%08x\r\n", get_r0());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r1: 0x%08x\r\n", get_r1());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r2: 0x%08x\r\n", get_r2());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r3: 0x%08x\r\n", get_r3());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r4: 0x%08x\r\n", get_r4());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r5: 0x%08x\r\n", get_r5());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r6: 0x%08x\r\n", get_r6());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r7: 0x%08x\r\n", get_r7());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r8: 0x%08x\r\n", get_r8());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r9: 0x%08x\r\n", get_r9());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r10: 0x%08x\r\n", get_r10());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r11: 0x%08x\r\n", get_r11());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r12: 0x%08x\r\n", get_r12());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r13: 0x%08x\r\n", get_r13());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r14: 0x%08x\r\n", get_r14());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "pc: 0x%08x\r\n", get_pc());
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "cpsr: 0x%08x\r\n", get_cspr());
    printlen(buffer, strlen(buffer));
}

void wrap_regs() {
    print_regs();
}

static inline void print_stack() {
    void* sp = get_sp();
    char buffer[50];

    int sp_min = -10*10;
    int sp_max = 10*10;

    int offset = 0;
    for (int i = sp_min; i < sp_max; i+=1) {
        offset = i;
        if (i == 0) {
            sprintf(buffer, "sp %x: with mem %X <--", sp+offset, *(int*)(sp+offset));
        } else {
            sprintf(buffer, "sp %x: with mem %X ", sp+offset, *(int*)(sp+offset));
        }
        printlen(buffer, strlen(buffer));
        printcrlf();
    }
}

static inline void print_stack2() {
    void* sp = get_sp();
    char buffer[50];

    int sp_min = -10*100;
    int sp_max = -10*50;

    int offset = 0;
    for (int i = sp_min; i < sp_max; i+=1) {
        offset = i;
        if (i == 0) {
            sprintf(buffer, "sp %x: with mem %c <--", sp+offset, *(char*)(sp+offset));
        } else {
            sprintf(buffer, "sp %x: with mem %c ", sp+offset, *(char*)(sp+offset));
        }
        printlen(buffer, strlen(buffer));
        printcrlf();
    }
}

void insert_hw_bpt(uint32_t addr) {
    asm("mov r0, %[to_break]" : : [to_break] "r" (addr));
    asm("mcr p14,0,r0,c0,c0, 4");
}

/**
 * @brief Overwrite the prefetch abort handler. The link register is not decremented by one instr.
 * 
 */
void overwrite_handler() {
    /*
        sub    lr, lr 0x0
        stmbd  sp!, {lr}
        nop
        nop
        nop
        nop
    */
    char* inject_nop = "\x00\xe0\x4e\xe2\x00\x40\x2d\xe9\x00\x00\x00\x00";
    // char* inject_branch_handler = "\x00\xe0\x4e\xe2\x00\x40\x2d\xe9\xc5\x20\x56\xeb";
    // char* inject = "\x00\xe0\x4e\xe2\x1e\xff\x2f\xe1\x00\x00\x00\x00";
    // char* inject_branch_crlf = "\x00\xe0\x4e\xe2\x00\x40\x2d\xe9\x5f\xe2\x39\xfa";
    // char* inject = "b\x00\xf0\x5e\xe2";
    write_memory(PREFETCH_ABORT, inject_nop, 12);

    memset(0x415983e4, 0, 12*4);
    return;
}

int task_main() {
    printcrlf();
    // print_hex(0x415983e0, 12*4+4+4);
    print_hex(0x400100bc, 12);
    overwrite_handler();
    printcrlf();
    print_hex(0x400100bc, 12);
    // print_hex(0x415983e0, 12*4+4+4);


    char* command = get_command();

    debug_command dc = parse_command(command);
    if (!strcmp(dc.command_type, "REG")) {
        print_regs();
    } else if (!strcmp(dc.command_type, "MEM")) {
        switch (dc.op) {
        case 'r':
            dump_byte_range(dc.memory_start, dc.memory_end);
            break;
        case 'w':
            write_memory(dc.memory_start, dc.payload, dc.payload_size);
            break;
        default:
            break;
        }
    }
    asm("bkpt");
    asm("nop");
    asm("nop");
    asm("nop");
    return 0;
}