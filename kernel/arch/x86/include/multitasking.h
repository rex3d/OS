#ifndef MULTITASKING_H
#define MULTITASKING_H

#include <stdint.h>

typedef enum {
    ARCH_16BIT,
    ARCH_32BIT,
    ARCH_64BIT
} arch_t;

typedef struct {
    arch_t arch;
    union {
        struct { uint16_t sp, bp; } t16;
        struct { uint32_t esp, ebp; } t32;
        struct { uint64_t rsp, rbp; } t64;
    };
} task_t;

#define MAX_TASKS 2
#define STACK_SIZE_16 4096
#define STACK_SIZE_32 4096
#define STACK_SIZE_64 8192

void multitasking_init();
void switch_task();

#endif
