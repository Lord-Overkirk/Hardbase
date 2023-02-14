#ifndef _STDLIB_H
#define _STDLIB_H

#define ATOI 0x40f0153d
#define PAL_TASK_SLEEP 0x40f0cd6d

#define atoi ((int (*)(char*))ATOI)

#define pal_tasksleep ((void (*)(uint32_t))PAL_TASK_SLEEP)


#endif // _STDLIB_H