@ Correction for thumb.
add         lr, lr,#0x1

handle_abort:
stmdb       sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}

@ @ Saving reg vals to mem.
@ r0
ldr         r0, [sp, #0x0]
movw        r1, #0x0066
movt        r1, #0x47d1
str         r0, [r1]


mov         r2, #52
mov         r3, #0x4

store_regs:
    @ r1 - r12
    ldr         r0, [sp, r3]
    add         r1, #0x4
    str         r0, [r1]

    add         r3, #0x4
    cmp         r2, r3
    bne         store_regs

@ sp
mrs         r3, cpsr            @ Switch to user mode, to make sure we can grab the values at the breakpoint.
mrs         r4, spsr
msr         cpsr_c, r4
mov         r0, sp
add         r1, #0x4
str         r0, [r1]

@ lr
mov         r0, lr
add         r1, #0x4
str         r0, [r1]
msr         cpsr_c, r3          @ Switch back to handler mode, the pc can be grabbed from the current link register.

@ pc
mov         r0, lr
sub         r0,#0x4
add         r1, #0x4
str         r0, [r1]

@ cpsr
mrs         r4, spsr
add         r1, #0x4
str         r4, [r1]
@ Registers saved!

add         r1, #0x4
mov         r6, r1

@ @ Calling get_running_task_id()
movw        r0, #0xa2b8
movt        r0, #0x4159
blx         r0

@ Calling get_current_task_pointer()
movw        r0, #0xb923
movt        r0, #0x4061
blx         r0
str         r0, [r6]

@ halt_all_tasks() via fun pointer table.
movw        r1, #0x0
movt        r1, #0x47cb
ldr         r1, [r1]
blx         r1

@ This point should not get reached, since halt_all_tasks() calls the scheduler.
return:
    @ cpsie   ifa
    ldmia       sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, pc}^

print_saved_regs:
    @ Call to print_regs via the function pointer table.
    movw         r5, 0x0
    movt         r5, 0x47ca
    ldr          r5, [r5]
    msr          cpsr_c, #0x33
    blx          r5
    msr          cpsr_c, #0x17

task_wait:
    movw         r5, 0xcd6d
    movt         r5, 0x40f0
    msr          cpsr_c, #0x33
    movw         r0, #0x86A0
    movt         r0, #0x0001
    blx          r5
    msr          cpsr_c, #0x17
    bx           lr


kernel_int_lock:
    mrs     r0, cpsr
    and     r0, #0x80
    cpsid   i
    bx      lr

kernel_int_unlock:
    tst     r0, #0x80
    bne     ret
    cpsie   i

ret:
    bx      lr

bl          kernel_int_lock
bl          kernel_int_unlock
