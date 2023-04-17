@ Check if running in arm or thumb.
@ mrs         r0, spsr
@ and         r0, r0, #32
@ cmp         r0, #32
@ beq         thumb_correction
@ b           arm_correction

@ thumb_correction:
    @ cpsid   ifa
    add         lr, lr,#0x1
    @ movw        lr, #0xb1f2
    @ movt        lr, #0x4060
@     b           handle_abort

@ arm_correction:
@     sub         lr, lr,#0x4
@     b           handle_abort


handle_abort:
stmdb       sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}

@ Mov crlf into r5.
@ movw         r5, 0x8a49
@ movt         r5, 0x40e8
@ @ msr         cpsr_c, #0x33
@ cpsid       if
@ @ blx          r5
@ @ blx          r5
@ @ blx          r5
@ @ msr         cpsr_c, #0x17

@ Restore
mov         r0, #0x2002
movw        r1, #0xcf20
movt        r1, #0x4061
strh        r0, [r1]

@ Restore dbg msg
mov         r0, #0xb40f
movw        r1, #0xb1ee
movt        r1, #0x4060
strh        r0, [r1]

mov         r0, #0xb088
movw        r1, #0xb1f4
movt        r1, #0x4060
strh        r0, [r1]

mov         r0, #0x4607
movw        r1, #0xb210
movt        r1, #0x4060
strh        r0, [r1]

@ Restore
@ mov         r0, #0xb57c
@ movw        r1, #0x7d00
@ movt        r1, #0x4068
@ strh        r0, [r1]

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
mrs r3, cpsr            @ Switch to user mode, to make sure we can grab the values at the breakpoint.
mrs r4, spsr
msr cpsr_c, r4
mov         r0, sp
add         r1, #0x4
str         r0, [r1]

@ lr
mov         r0, lr
add         r1, #0x4
str         r0, [r1]
msr cpsr_c, r3          @ Switch back to handler mode, the pc can be grabbed from the current link register.

@ pc
mov         r0, lr
add         r0,#0x0
add         r1, #0x4
str         r0, [r1]

mrs         r4, spsr
add         r1, #0x4
str         r4, [r1]

@ Copy spsr cpsr
@ mrs         r4, cpsr
@ MRS R0, spsr
@ @ ORR R0, R0, #0x12
@ MSR CPSR, R0
@ cpsid ia


@ MSR CPSR_c, R4
@ bl print_saved_regs
@ cpsr
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

@ add         r6, #0x4
@ str         r0, [r6]

@ halt_all_tasks() via fun pointer table.
movw        r1, #0x0
movt        r1, #0x47cb
@ add         r1, #0x4
ldr         r1, [r1]
@ str         r1, [r6]

movw        r1, #0x0369
@ movw        r1, #0x00b9
movt        r1, #0x47c0
@ Offset for ARM Thumb
@ add         r1, #0x1
@ msr          cpsr_c, #0x33
blx         r1
@ msr          cpsr_c, #0x17

@ cmp         r0, #0x0
@ beq         return

@ cpsid       i
@ cpsie       af
@ cps         #0x13

@ cpsie       i, #0x17
@ add         r1, #0x4
@ str         r0, [r1]
@ mov         r4, r0

@ movw        r0, #0x0EE8
@ movt        r0, #0x0480
@ ldr         r0, [r0]
@ add         r1, #0x4
@ str         r0, [r1]


@ @ Calling get_tmu_from_task_id()
@ @ movw        r0, #0xa28c
@ @ movt        r0, #0x4159
@ @ blx         r0
@ @ mov         r3, r0

@ @ bl          kernel_int_lock
@ @ bl          kernel_int_unlock

@ AT pointer

@ movw        r0, #0xb238
@ movt        r0, #0x4161
@ mrs         r4, spsr
@ msr         cpsr_cxsf, r4

@ Call task_block !
@ movw        r2, #0xb8db
@ movt        r2, #0x4061
@ blx         r2

@ movw        r1, #0x0066
@ movt        r1, #0x47d1

@ movw        r2, #0x2654
@ movt        r2, #0x0480
@ mov         r3, #0x4
@ mov         r4, #0xa5
@ mul         r0, r4, r3
@ add         r0, r0, r2
@ @ ldr         r0, [r0]

@ str         r0, [r1]

@ movw        r2, #0x8668
@ movt        r2, #0x4159
@ mov         r0, #0xa5
@ bl          r2

@@ Call task_start
@ movw        r2, #0xb8df
@ movt        r2, #0x4061
@ blx         r2
@ ldmia       sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}^


@ @ Call task_suspend
@ eor         r2, r2
@ mov         r2, #0x2
@ eor         r1, r1
@ mov         r1, #0x1
@ mov         r0, #0x46
@ movw        r5, #0xa3e4
@ movt        r5, #0x4159
@ bl          r5


@ cpsie       if
@ bl task_wait
@ bl task_wait
@ bl task_wait
@ bl task_wait
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
