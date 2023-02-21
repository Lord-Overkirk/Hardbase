subs         lr, lr,#0x0
stmdb       sp!, {r0, r1, r2, r3, r4, r5, lr}

@ Set cpsr reg values to before BKPT.
msr         cpsr_f, #0x60000000
msr         cpsr_x, #0x00000000
msr         cpsr_c, #0x17

mrs         r0, spsr
movw        r1, #0x006a
movt        r1, #0x47d0
str         r0, [r1]


@ Mov crlf into r5.
movw         r5, 0x8a49
movt         r5, 0x40e8
msr         cpsr_c, #0x33
blx          r5
blx          r5
blx          r5
msr         cpsr_c, #0x17

mrs         r0, cpsr
movw        r1, #0x006e
movt        r1, #0x47d0
str         r0, [r1]

ldmia       sp!, {r0, r1, r2, r3, r4, r5, pc}^



@ blx          r5
@ blx          r5
@ blx          r5
@ blx          r5
