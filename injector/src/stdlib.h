#ifndef _STDLIB_H
#define _STDLIB_H

#define ATOI 0x40f0153d
#define PAL_TASK_SLEEP 0x40f0cd6d
#define PAL_TASK_GET_ID 0x40f0ca35
#define GET_RUNNING_TASK_ID 0x4159a2b8
#define TASK_SUSPEND 0x4159a3e4
#define OS_GET_CURRENT_TASK_POINTER 0x4061b923
#define PAL_TASK_GET_CURRENT_ID 0x40f0ca35
#define PAL_TASK_GET_NAME 0x40f0d2b7
#define PAL_TASK_GET_NUMBER 0x40f0d2c5
#define OS_GET_CURRENT_TASK 0x40a21ca9
#define INTERRUPTS_DISABLE 0x40f011c4
#define ARMOS_INTERRUPT_CONTROL 0x4061bbaf
#define TASK_BLOCK 0x41599acc
#define TASK_START 0x4159a0d8
#define TASK_DELETE 0x41599c60
#define SCHEDULE 0x41598668
#define SET_PRIO_BIT 0x4159cffc

#define atoi ((int (*)(char*))ATOI)
#define pal_tasksleep ((void (*)(uint32_t))PAL_TASK_SLEEP)
#define pal_taskgetid ((uint32_t (*)(void))PAL_TASK_GET_ID)
#define get_running_task_id ((uint32_t (*)(void))GET_RUNNING_TASK_ID)
#define task_suspend ((void (*) (uint32_t, uint32_t, uint32_t)) TASK_SUSPEND)
#define os_get_current_task_pointer ((task*(*) (void)) OS_GET_CURRENT_TASK_POINTER)
#define pal_task_get_current_id ((short(*) (void)) PAL_TASK_GET_CURRENT_ID)
#define pal_task_get_name ((char(*) (task*)) PAL_TASK_GET_NAME)
#define pal_task_get_number ((uint32_t(*) (task*)) PAL_TASK_GET_NUMBER)
#define os_get_current_task ((uint32_t(*) (void)) OS_GET_CURRENT_TASK)
#define interrupts_disable ((uint32_t(*) (void)) INTERRUPTS_DISABLE)
#define armos_interrupt_control ((uint32_t(*) (uint32_t)) ARMOS_INTERRUPT_CONTROL)
#define task_block ((uint32_t(*) (task*)) TASK_BLOCK)
#define task_start ((uint32_t(*) (task*)) TASK_START)
#define task_delete ((uint32_t(*) (task*)) TASK_DELETE)
#define schedule ((void(*) (uint32_t)) SCHEDULE)
#define set_priority_bit ((void(*) (uint32_t)) SET_PRIO_BIT)


typedef struct task {
    struct task* next_task;
    struct task* prev_task;
    uint32_t magic;
    uint16_t task_id;
    uint16_t task_id_min_1;
    char unknown1[0xa];
    uint32_t _stackbase;
    uint32_t _stack_ptr;
    char unknown2[0x6];
    uint32_t stack_top;
    uint32_t stack_base;
    uint32_t stack_ptr;
    uint32_t is_running;
    char unknown3[0x18];
    uint32_t saved_stack_ptr;
    char unknown4[0x8];
    char* name;
    char* other_name;
    char rest[0xd7];
} __attribute__((packed)) task;

#endif // _STDLIB_H