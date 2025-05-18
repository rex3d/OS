// kheap.c – wersja do testów, używa malloc
#include "kheap.h"
#include <stdlib.h>

void *kmalloc(size_t size) {
    return malloc(size);
}

void kfree(void *ptr) {
    free(ptr);
}
