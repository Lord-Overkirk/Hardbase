#include "shannon.h"
#include "registers.h"
#include "assert.h"
#include "debug_command.h"
#include "string.h"

#define TASK_COUNT 0x411a9439

#define ARG 0x43050B09

#define TASK_TABLE_COUNT 0x41612cc0

const char TASK_NAME[] = "DEBUG\0";


int (*task_count)(void) = (void*) TASK_COUNT;
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

void debugger_hook() {
    static int beenhere = 0;

    if (beenhere != 2){
        char* a = "YOHAY1";
        size_t y_len = strlen(a);

        char buffer[50];
        int d = 10, b = 20, c;
        c = d + b;
        sprintf(buffer, "Sum of %d and %d is %d", d, b, task_count());
        printlen(buffer, strlen(buffer));

        printcrlf();
        print_hex(a, 7);
        printcrlf();

        dump_byte_range(0x40669586, 0x4066959f);
        printcrlf();
        dump_byte_range(0x4066959f, 0x40669586);
        printcrlf();
        // print_hex(get_pc(), 4);
        char buffer2[50];
        sprintf(buffer2, "The pc is: %x", get_pc());
        printlen(buffer2, strlen(buffer2));

        char buffer3[50];
        sprintf(buffer3, "The r25 is: %x", get_r3());
        printlen(buffer3, strlen(buffer3));
        printcrlf();

        beenhere++;
    }
    return;
}

static inline void print_regs() {
    char buffer[50];
    sprintf(buffer, "r0: %x", get_sp());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r1: %x", *(int*)get_r1());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r2: %x", *(int*)get_r2());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r3: %x", *(int*)get_r3());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r4: %x", *(int*)get_r4());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r5: %x", *(int*)get_r5());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r6: %x", *(int*)get_r6());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r7: %x", *(int*)get_r7());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r8: %x", get_r8());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r9: %x", *(int*)get_r9());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r10: %x", get_r10());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r11: %x", *(int*)get_r11());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r12: %x", get_r12());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r13: %x", *(int*)get_r13());
    printlen(buffer, strlen(buffer));
    printcrlf();
    sprintf(buffer, "r14: %x", *(int*)get_r14());
    printlen(buffer, strlen(buffer));
    printcrlf();
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
        // print_hex(sp+offset, 4);
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
        // print_hex(sp+offset, 4);
        printcrlf();
    }
}

int task_main() {
    printcrlf();
    printlen("START HIER", 11);
    printcrlf();

    char* command = get_command();
    printlen(command, strlen(command));
    printcrlf();
    print_hex(command, strlen(command));

    return 0;
}