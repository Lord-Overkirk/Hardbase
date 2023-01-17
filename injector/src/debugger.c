#include "shannon.h"
#include "registers.h"

// Address includes THUMB bit
#define PRINTBUF 0x40e895e9
#define PRINTCRLF 0x40e88a49
#define STRLEN 0x40f146a5
#define SPRINTF 0x40f0c891
#define TASK_COUNT 0x411a9439

#define TASK_TABLE_COUNT 0x41612cc0

const char TASK_NAME[] = "DEBUG\0";

void (*printlen)(char *, int) = (void*) PRINTBUF;
size_t (*strlen)(char*) = (void*) STRLEN;
void (*printcrlf)(void) = (void*) PRINTCRLF;
int (*sprintf)(char*, char*, ...) = (void*) SPRINTF;
int (*task_count)(void) = (void*) TASK_COUNT;
int (*ch_select2)(void) = (void*) 0x40aad8d9;
void (*ch_select)(int) = (void*) 0x40b67357;



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
    char buffer2[50];
    sprintf(buffer2, "r0: %x", get_sp());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r1: %x", *(int*)get_r1());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r2: %x", *(int*)get_r2());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r3: %x", *(int*)get_r3());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r4: %x", *(int*)get_r4());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r5: %x", *(int*)get_r5());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r6: %x", *(int*)get_r6());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r7: %x", *(int*)get_r7());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r8: %x", get_r8());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r9: %x", *(int*)get_r9());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r10: %x", get_r10());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r11: %x", *(int*)get_r11());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r12: %x", get_r12());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r13: %x", *(int*)get_r13());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
    sprintf(buffer2, "r14: %x", *(int*)get_r14());
    printlen(buffer2, strlen(buffer2));
    printcrlf();
}

static inline void print_stack() {
    void* sp = get_sp();
    char buffer[50];

    size_t offset = 0;
    for (int i = -10; i < 10; i++) {
        offset = i * 4;
        sprintf(buffer, "sp %x[+%d]: ", sp+offset, offset);
        printlen(buffer, strlen(buffer));
        print_hex(sp+offset, 4);
        printcrlf();
    }
}

int task_main() {
    // print_stack();

    // char * msg = "DEBUGGER CODE GOES HERE";
    // int msg_len = 23;
    // void (*printlen)(char *, int) = (void*) PRINTBUF;
    // void (*printcrlf)(void) = (void*) PRINTCRLF;

    // printcrlf();
    // printlen("START", 5);

    // sprintf(buffer2, "r1 is: %x", *(int*)get_r1());
    // printlen(buffer2, strlen(buffer2));
    // printcrlf();

    // sprintf(buffer2, "r2 is: %x", *(int*)get_r2());
    // printlen(buffer2, strlen(buffer2));;
    // printcrlf();

    // sprintf(buffer2, "r3 is: %x", *(int*)get_r3());
    // printlen(buffer2, strlen(buffer2));;
    // printcrlf();

    // sprintf(buffer2, "sp is: %x", *(int*)get_r13());
    // printlen(buffer2, strlen(buffer2));;
    // printcrlf();

    // // print_hex(0x41948f40, 4);
    // // print_hex(&something, 4);
    // // int one = 1;
    print_hex(0x41956ae0, 4);
    printcrlf();
    print_hex(*(char*)0x41956ae0, 4);
    printcrlf();
    ch_select2();
    // // ch_select(3);
    // // ch_select2();
    // // printlen("STAR2", 5);
    // printcrlf();

    // printlen(msg, msg_len);
    // printcrlf();
    // debugger_hook();
    return 0;
}