static __inline__ void * get_pc(void) {
    void *pc;
    asm("mov %0, pc" : "=r"(pc));
    return pc;
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