struct arm_registers {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    uint32_t cpsr;
    uint32_t task_ptr;
} __attribute__((packed));

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

static __inline__ void * get_cpsr(void) {
    void *reg;
    // asm volatile ("mov %0, cpsr" : "=r"(reg));
    asm volatile (" mrs %0, cpsr" : "=r" (reg) :  );
    return reg;
}

static __inline__ void * get_sp(void) {
    void *reg;
    asm("mov %0, sp" : "=r"(reg));
    return reg;
}
