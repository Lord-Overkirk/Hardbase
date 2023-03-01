#include "str.h"
#include "shannon.h"
#include "registers.h"
#include "assert.h"
#include "debug_command.h"
#include "stdlib.h"
#include <common.h>

#define PREFETCH_ABORT 0x400100bc
#define PRINT_REGS_TABLE 0x47ca0000


const char TASK_NAME[] = "DEBUG\0";

int (*ch_select)(void) = (void*) 0x40aad8d9;

/**
 * We use this as a function table with pointers to functions
 * that need to be called from the custom prefetch abort
 * exception handler.
 */
int init_done = 0;
void (*fun_pointer_vector[1]) (void);



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

/**
 * @brief Send all the register contents to the gdbserver.
 * 
 */
inline static volatile void print_regs() {
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

void print_saved_regs() {
    char buffer[50];
    struct arm_registers* rs = (struct arm_registers*)0x47d10066;
    
    sprintf(buffer, "r0: 0x%08x\r\n", rs->r0);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r1: 0x%08x\r\n", rs->r1);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r2: 0x%08x\r\n", rs->r2);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r3: 0x%08x\r\n", rs->r3);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r4: 0x%08x\r\n", rs->r4);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r5: 0x%08x\r\n", rs->r5);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r6: 0x%08x\r\n", rs->r6);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r7: 0x%08x\r\n", rs->r7);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r8: 0x%08x\r\n", rs->r8);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r9: 0x%08x\r\n", rs->r9);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r10: 0x%08x\r\n", rs->r10);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r11: 0x%08x\r\n", rs->r11);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "r12: 0x%08x\r\n", rs->r12);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "sp: 0x%08x\r\n", rs->sp);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "lr: 0x%08x\r\n", rs->lr);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "pc: 0x%08x\r\n", rs->pc);
    printlen(buffer, strlen(buffer));
    sprintf(buffer, "cpsr: 0x%08x\r\n", rs->cpsr);
    printlen(buffer, strlen(buffer));
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

int task_main() {
    // Only load this function pointer table the first time.
    if (!init_done) {
        fun_pointer_vector[0] = print_saved_regs;
        memcpy((void*)PRINT_REGS_TABLE, fun_pointer_vector, sizeof(fun_pointer_vector));
        init_done = 1;
    }
    printcrlf();
    printcrlf();

    char* command = get_command();

    debug_command dc = parse_command(command);
    if (!strcmp(dc.command_type, "REG")) {
        asm("nop");
        printcrlf();
        // asm("mov r0, r2");
        // asm("bkpt");
        asm("nop");
        asm("nop");
        printcrlf();
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
    return 0;
}