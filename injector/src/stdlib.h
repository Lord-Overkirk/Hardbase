#ifndef _STDLIB_H
#define _STDLIB_H

#define ATOI 0x40f0153d
#define PAL_TASK_SLEEP 0x40f0cd6d
#define PAL_TASK_GET_ID 0x40f0ca35
#define GET_RUNNING_TASK_ID 0x4159a2b8
#define TASK_SUSPEND 0x4159a3e4

#define atoi ((int (*)(char*))ATOI)
#define pal_tasksleep ((void (*)(uint32_t))PAL_TASK_SLEEP)
#define pal_taskgetid ((uint32_t (*)(void))PAL_TASK_GET_ID)
#define get_running_task_id ((uint32_t (*)(void))GET_RUNNING_TASK_ID)
#define task_suspend ((void (*) (int, uint32_t, uint32_t)))

#endif // _STDLIB_H