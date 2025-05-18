#include "multitasking.h"
#include "video.h"

#define SUPPORT_64BIT 1 // ustaw na 0 jeśli nie chcesz 64-bit

static task_t tasks[MAX_TASKS];
static int current_task = 0;

static uint8_t stack_16[MAX_TASKS][STACK_SIZE_16];
static uint8_t stack_32[MAX_TASKS][STACK_SIZE_32];

#if SUPPORT_64BIT
static uint8_t stack_64[MAX_TASKS][STACK_SIZE_64];
extern void switch_task64_asm(uint64_t* old_rsp, uint64_t* old_rbp, uint64_t new_rsp, uint64_t new_rbp);
#endif

extern void switch_task16_asm(uint16_t* old_sp, uint16_t* old_bp, uint16_t new_sp, uint16_t new_bp);
extern void switch_task32_asm(uint32_t* old_esp, uint32_t* old_ebp, uint32_t new_esp, uint32_t new_ebp);

// Inicjalizacja zadań i stosów
static inline void multitasking_init(void) {
    video_print("Multitasking init (16/32/64)\n");
    for (int i = 0; i < MAX_TASKS; i++) {
#if SUPPORT_64BIT
        tasks[i].arch = ARCH_64BIT;
        tasks[i].t64.rsp = (uint64_t)&stack_64[i][STACK_SIZE_64 - sizeof(uint64_t)];
        tasks[i].t64.rbp = tasks[i].t64.rsp;
#else
        tasks[i].arch = ARCH_32BIT;
        tasks[i].t32.esp = (uint32_t)&stack_32[i][STACK_SIZE_32 - sizeof(uint32_t)];
        tasks[i].t32.ebp = tasks[i].t32.esp;
#endif
    }
}

// Przełączanie zadań
static inline void switch_task(void) {
    int prev = current_task;
    current_task = (current_task + 1) % MAX_TASKS;

    switch (tasks[prev].arch) {
        case ARCH_16BIT:
            switch_task16_asm(&tasks[prev].t16.sp, &tasks[prev].t16.bp,
                              tasks[current_task].t16.sp, tasks[current_task].t16.bp);
            break;
        case ARCH_32BIT:
            switch_task32_asm(&tasks[prev].t32.esp, &tasks[prev].t32.ebp,
                              tasks[current_task].t32.esp, tasks[current_task].t32.ebp);
            break;
#if SUPPORT_64BIT
        case ARCH_64BIT:
            switch_task64_asm(&tasks[prev].t64.rsp, &tasks[prev].t64.rbp,
                              tasks[current_task].t64.rsp, tasks[current_task].t64.rbp);
            break;
#endif
        default:
            // Obsługa nieznanej architektury
            break;
    }
}