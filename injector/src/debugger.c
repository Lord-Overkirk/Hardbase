#include "str.h"
#include "shannon.h"
#include "registers.h"
#include "assert.h"
#include "debug_command.h"
#include "stdlib.h"
#include <common.h>

#define PREFETCH_ABORT 0x400100bc
#define PRINT_REGS_TABLE 0x47ca0000
#define HALT_ALL_TASKS 0x47cb0000
#define TASK_BASE 0x04802654

const char TASK_NAME[] = "DEBUG\0";

int (*ch_select)(void) = (void *)0x40aad8d9;

/**
 * We use this as a function table with pointers to functions
 * that need to be called from the custom prefetch abort
 * exception handler.
 */
int init_done = 0;
void (*fun_pointer_vector[1])(task *);

/* Print len bytes as hexadecimals from specified addr. */
void print_hex(char *in, int len)
{
    for (int i = 0; i < len; i++)
    {
        char b = in[i];
        char h = (b >> 4) & 0xf;
        char l = b & 0xf;

        char r1 = (h >= 10 ? 'a' + (h - 10) : '0' + h);
        char r2 = (l >= 10 ? 'a' + (l - 10) : '0' + l);
        char s1[1] = {r1};
        char s2[1] = {r2};

        printlen(s1, 1);
        printlen(s2, 1);
        printlen(" ", 1);
    }
}

static inline void write_memory(unsigned int start, char *payload, unsigned int size)
{
    volatile char *dst = (char *)start;
    memcpy((void *)dst, payload, size);
}

/* Dump the bytes as hex in the specified range. */
void dump_byte_range(unsigned int start, unsigned int end)
{
    if (start > end)
    {
        char *err_str = "Error, wrong range\n";
        printlen(err_str, strlen(err_str));
        return;
    }
    // ASSERT(start > end);
    int stride = 16;
    int remaining_bytes = end-start % 16;
    for (int i = 0; i < end-start; i+=stride) {
        printlen(start+i, stride);
    }
    printlen(end - remaining_bytes, remaining_bytes);
    // print_hex((char *)start, end - start);
}

/**
 * @brief Send all the register contents to the gdbserver.
 *
 */
inline static void store_regs()
{
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

void print_saved_regs()
{
    char buffer[50];
    struct arm_registers *rs = (struct arm_registers *)0x47d10066;

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
    sprintf(buffer, "task_ptr: 0x%08x\r\n", rs->task_ptr);
    printlen(buffer, strlen(buffer));
}

void print_task(task *task)
{
    char buffer[300];
    sprintf(buffer, " Base: 0x%08x\r\n task_next: 0x%08x\r\n task_id: 0x%08x\r\n task_id-1: 0x%08x\r\n name: %s other_name: %s\r\n is_running: %d\r\n stack_top: 0x%08x\r\n stack_base: 0x%08x\r\n stack_ptr: 0x%08x\r\n",
            task, task->next_task, task->task_id, task->task_id_min_1, &task->name, &task->other_name, task->is_running, task->stack_top, task->stack_base, task->stack_ptr);
    printlen(buffer, strlen(buffer));
    printcrlf();
}

int is_kernel_task(uint32_t task_id)
{
    if (task_id == 0x420)
    {
        return 1;
    }
    if (task_id > 7)
    {
        return 0;
    }
    return 1;
}

void list_tasks()
{
    task *base_task = (task *)(*(uint32_t *)(0x04802654 + 0x1 * 4));

    while (base_task->task_id != 0)
    {
        if (base_task->name == 0)
        {
            return;
        }
        print_task(base_task);
        base_task = base_task->next_task;
    }
}

/**
 * @brief This function should only be called from the custom prefetch abort exception handler.
 * After the handler saves the register values, this function is called in order to suspend al non-kernel tasks.
 * One excpetion is the ATI task (id: 0xa6) that is responsible for serial communication.
 *
 * @param curr_task, the task that now contains the bkpt instruction.
 */
void halt_all_tasks(task *curr_task)
{
    task *next_task = (task *)(*(uint32_t *)(TASK_BASE + 0x1 * 4));
    task *first_task = next_task;

    while (next_task->next_task != first_task)
    {
        if (next_task->name == 0)
        {
            break;
        }
        if (is_kernel_task(next_task->task_id) || next_task->task_id == 0xa6 || curr_task == next_task)
        {
            next_task = next_task->next_task;
            continue;
        }
        task_block(next_task);
        next_task = next_task->next_task;
    }

    // Finally, we block the current task if it was no kernel task.
    if (!is_kernel_task(curr_task))
    {
        task_block(curr_task);
    }
}

static inline void print_stack()
{
    void *sp = get_sp();
    char buffer[50];

    int sp_min = -10 * 10;
    int sp_max = 10 * 10;

    int offset = 0;
    for (int i = sp_min; i < sp_max; i += 1)
    {
        offset = i;
        if (i == 0)
        {
            sprintf(buffer, "sp %x: with mem %X <--", sp + offset, *(int *)(sp + offset));
        }
        else
        {
            sprintf(buffer, "sp %x: with mem %X ", sp + offset, *(int *)(sp + offset));
        }
        printlen(buffer, strlen(buffer));
        printcrlf();
    }
}

static inline void print_stack2()
{
    void *sp = get_sp();
    char buffer[50];

    int sp_min = -10 * 100;
    int sp_max = -10 * 50;

    int offset = 0;
    for (int i = sp_min; i < sp_max; i += 1)
    {
        offset = i;
        if (i == 0)
        {
            sprintf(buffer, "sp %x: with mem %c <--", sp + offset, *(char *)(sp + offset));
        }
        else
        {
            sprintf(buffer, "sp %x: with mem %c ", sp + offset, *(char *)(sp + offset));
        }
        printlen(buffer, strlen(buffer));
        printcrlf();
    }
}

void insert_hw_bpt(uint32_t addr)
{
    asm("mov r0, %[to_break]"
        :
        : [to_break] "r"(addr));
    asm("mcr p14,0,r0,c0,c0, 4");
}

int task_main()
{
    asm("cpsid ifa");
    printcrlf();

    // Only load the function pointer table the first time.
    if (!init_done)
    {
        fun_pointer_vector[0] = halt_all_tasks;
        memcpy((void *)HALT_ALL_TASKS, fun_pointer_vector, sizeof(fun_pointer_vector));
        store_regs();
        init_done = 1;
    }

    char *command = get_command();

    debug_command dc = parse_command(command);
    if (!strcmp(dc.command_type, "REG"))
    {
        print_saved_regs();
    }
    else if (!strcmp(dc.command_type, "MEM"))
    {
        switch (dc.op)
        {
        case 'r':
            dump_byte_range(dc.memory_start, dc.memory_end);
            break;
        case 'w':
            asm("cpsid if");
            write_memory(dc.memory_start, dc.payload, dc.payload_size);
            printcrlf();
            break;
        default:
            break;
        }
    }
    asm("cpsie ifa");
    return 0;
}