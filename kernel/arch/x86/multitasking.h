#ifndef MULTITASKING_H
#define MULTITASKING_H

#include <stdint.h>

#define MAX_TASKS      8
#define STACK_SIZE_16  512
#define STACK_SIZE_32  2048
#define STACK_SIZE_64  4096

typedef enum {
    ARCH_16BIT,
    ARCH_32BIT,
    ARCH_64BIT
} arch_t;

typedef struct {
    uint16_t sp, bp;
} task16_t;

typedef struct {
    uint32_t esp, ebp;
} task32_t;

typedef struct {
    uint64_t rsp, rbp;
} task64_t;

typedef struct {
    arch_t arch;
    union {
        task16_t t16;
        task32_t t32;
        task64_t t64;
    };
} task_t;

// Inicjalizacja systemu wielozadaniowego
static inline void multitasking_init(void);

// Przełącza na kolejne zadanie
static inline void switch_task(void);

#endif // MULTITASKING_H