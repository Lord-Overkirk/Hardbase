subs         lr, lr,#0x0
stmdb       sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}

@ Set cpsr reg values to before BKPT.
msr         cpsr_f, #0x60000000
msr         cpsr_x, #0x00000000
msr         cpsr_c, #0x17

@ Mov crlf into r5.
movw         r5, 0x8a49
movt         r5, 0x40e8
msr         cpsr_c, #0x33
blx          r5
blx          r5
blx          r5
msr         cpsr_c, #0x17

@ Saving reg vals to mem.

@ r0
ldr         r0, [sp, #0x0]
movw        r1, #0x0066
movt        r1, #0x47d1
str         r0, [r1]

@ eor         r2, r2
@ eor         r3, r3
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
sub         r0,#0x4
add         r1, #0x4
str         r0, [r1]

@ cpsr
mrs         r0, spsr
add         r1, #0x4
str         r0, [r1]

@ Call to print_regs via the function pointer table.
movw         r5, 0x0
movt         r5, 0x47ca
ldr         r5, [r5]
msr         cpsr_c, #0x33
blx          r5

msr         cpsr_c, #0x17

ldmia       sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, pc}^
