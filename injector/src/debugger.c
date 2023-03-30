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
inline static void store_regs() {
    asm("stmdb sp!, {r0-r12}");

    asm("movw r1, #0x0066");
    asm("movt r1, #0x47d1");

    // r0
    asm("ldr r0, [sp, #0]");
    asm("str r0, [r1]");

    asm("mov r2, #52 \n\t"
        "mov r3, #0x4 \n\t"

        // r1 - r12
        "store_regs:"
            "ldr r0, [sp, r3] \n\t"
            "add r1, #0x4 \n\t"
            "str r0, [r1] \n\t"

            "add r3, #0x4 \n\t"
            "cmp r2, r3 \n\t"
            "bne store_regs");

    asm("mov r0, sp");
    asm("add r0, #52");
    asm("add r1, #0x4");
    asm("str r0, [r1]");

    asm("mov r0, lr");
    asm("add r1, #0x4");
    asm("str r0, [r1]");

    asm("mov r0, pc");
    asm("add r1, #0x4");
    asm("str r0, [r1]");

    asm("mrs r0, cpsr");
    asm("add r1, #0x4");
    // We know we are in Thumb, so we set the masked thumb bit manually.
    asm("orr r0, r0, #0x20");
    asm("str r0, [r1]");

    asm("ldmia sp!, {r0-r12}");
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

void interrupt_enabled() {

}

int task_main() {
    asm("cpsid if");
    printcrlf();
    // Only load this function pointer table the first time.
    if (!init_done) {
        // uint32_t running_task_id = get_running_task_id();
        uint32_t r2 = os_get_current_task_pointer();
        uint32_t running_task_id = *(uint32_t*)0x04800ee8;
        char buff_id[50];
        sprintf(buff_id, "running task id: 0x%08x\r\n", r2);
        printlen(buff_id, strlen(buff_id));
        print_hex(r2, 0x30);

        uint32_t t0 = *(uint32_t*) (0x04802654 + 0x46 * 4);
        // uint32_t t1 = (uint32_t) os_get_current_task_pointer() + 0x4;
        uint32_t t1 = *(uint32_t*) (0x04802654 + running_task_id * 4);
        char buff0[200];
        char buff1[200];
        for (int i = 0; i < 0x1; i++) {
            sprintf(buff0, "next addr: 0x%08x running: 0x%08x id: 0x%08x %s\r\n", t0, *(uint32_t*)(t0+0x34), *(short*)(t0+0x0c), t0+0x5c);
            sprintf(buff1, "prev addr: 0x%08x running: 0x%08x id: 0x%08x %s\r\n", t1, *(uint32_t*)(t1+0x34), *(short*)(t1+0x0c), t1+0x5c);

            printlen(buff0, strlen(buff0));
            printlen(buff1, strlen(buff1));
            t0 = *(uint32_t*)t0;
            t1 = *(uint32_t*)t1;
        }

        fun_pointer_vector[0] = print_saved_regs;
        memcpy((void*)PRINT_REGS_TABLE, fun_pointer_vector, sizeof(fun_pointer_vector));
        init_done = 1;
    }

    char* command = get_command();

    debug_command dc = parse_command(command);
    if (!strcmp(dc.command_type, "REG")) {
        asm("nop");
        // asm("mrs r0, cpsr");
        // asm("and r0, r0, #0x20");
        // asm("tst r0, #0x20");
        // asm("mrs r0, spsr");
        // asm("ldr r1, =0x1f");
        // asm("and r0, r0, r1"); 
        // store_regs();
        print_saved_regs();
        // asm("bkpt");
        asm("nop");
        asm("nop");
    } else if (!strcmp(dc.command_type, "MEM")) {
        switch (dc.op) {
        case 'r':
            dump_byte_range(dc.memory_start, dc.memory_end);
            break;
        case 'w':
            asm("cpsid if");
            write_memory(dc.memory_start, dc.payload, dc.payload_size);
            // dump_byte_range(dc.memory_start, dc.memory_start+2);
            break;
        default:
            break;
        }
    }
    // asm("cpsie if");
    return 0;
}