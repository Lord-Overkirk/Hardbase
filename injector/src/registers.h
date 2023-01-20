static __inline__ void * get_pc(void) {
    void *pc;
    asm("mov %0, pc" : "=r"(pc));
    return pc;
}

static __inline__ void * get_r0(void) {
    void *reg;
    asm("mov %0, r0" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r1(void) {
    void *reg;
    asm("mov %0, r1" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r2(void) {
    void *reg;
    asm("mov %0, r2" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r3(void) {
    void *reg;
    asm("mov %0, r3" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r4(void) {
    void *reg;
    asm("mov %0, r4" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r5(void) {
    void *reg;
    asm("mov %0, r5" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r6(void) {
    void *reg;
    asm("mov %0, r6" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r7(void) {
    void *reg;
    asm("mov %0, r7" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r8(void) {
    void *reg;
    asm("mov %0, r8" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r9(void) {
    void *reg;
    asm("mov %0, r9" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r10(void) {
    void *reg;
    asm("mov %0, r10" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r11(void) {
    void *reg;
    asm("mov %0, r11" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r12(void) {
    void *reg;
    asm("mov %0, r12" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r13(void) {
    void *pc;
    asm("mov %0, r13" : "=r"(pc));
    return pc;
}

static __inline__ void * get_r14(void) {
    void *reg;
    asm("mov %0, r14" : "=r"(reg));
    return reg;
}

static __inline__ void * get_r15(void) {
    void *pc;
    asm("mov %0, r15" : "=r"(pc));
    return pc;
}

static __inline__ void * get_r25(void) {
    void *pc;
    asm("mov %0, r25" : "=r"(pc));
    return pc;
}

static __inline__ void * get_cspr(void) {
    void *pc;
    asm("mov %0, cspr" : "=r"(pc));
    return pc;
}

static __inline__ void * get_sp(void) {
    void *reg;
    asm("mov %0, sp" : "=r"(reg));
    return reg;
}
