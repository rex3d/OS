#include "slab_allocator.h"
#include <stdlib.h>
#include <stdint.h>

typedef struct slab {
    struct slab* next;
    void* free_list;
    size_t free_count;
    void* mem_start; // początek zaalokowanego bloku
} slab_t;

struct slab_cache {
    size_t obj_size;
    size_t objs_per_slab;
    slab_t* slabs;
};

static slab_t* slab_create(size_t obj_size, size_t objs_per_slab) {
    slab_t* slab = (slab_t*)malloc(sizeof(slab_t));
    if (!slab) return NULL;
    size_t slab_bytes = obj_size * objs_per_slab;
    slab->mem_start = malloc(slab_bytes);
    if (!slab->mem_start) {
        free(slab);
        return NULL;
    }
    slab->free_list = slab->mem_start;
    slab->free_count = objs_per_slab;
    slab->next = NULL;

    // Inicjalizacja wolnej listy
    uintptr_t ptr = (uintptr_t)slab->mem_start;
    for (size_t i = 0; i < objs_per_slab - 1; ++i) {
        uintptr_t next_ptr = ptr + obj_size;
        *(void**)ptr = (void*)next_ptr;
        ptr = next_ptr;
    }
    *(void**)ptr = NULL;
    return slab;
}

slab_cache_t* slab_cache_create(size_t obj_size, size_t objs_per_slab) {
    if (obj_size < sizeof(void*)) obj_size = sizeof(void*);
    slab_cache_t* cache = (slab_cache_t*)malloc(sizeof(slab_cache_t));
    if (!cache) return NULL;
    cache->obj_size = obj_size;
    cache->objs_per_slab = objs_per_slab;
    cache->slabs = NULL;
    return cache;
}

void slab_cache_destroy(slab_cache_t* cache) {
    slab_t* slab = cache->slabs;
    while (slab) {
        slab_t* next = slab->next;
        free(slab->mem_start);
        free(slab);
        slab = next;
    }
    free(cache);
}

void* slab_alloc(slab_cache_t* cache) {
    slab_t* slab = cache->slabs;
    while (slab && slab->free_count == 0)
        slab = slab->next;

    if (!slab) {
        slab = slab_create(cache->obj_size, cache->objs_per_slab);
        if (!slab) return NULL;
        slab->next = cache->slabs;
        cache->slabs = slab;
    }

    void* obj = slab->free_list;
    slab->free_list = *(void**)obj;
    slab->free_count--;
    return obj;
}

void slab_free(slab_cache_t* cache, void* obj) {
    slab_t* slab = cache->slabs;
    while (slab) {
        uintptr_t start = (uintptr_t)slab->mem_start;
        uintptr_t end = start + cache->obj_size * cache->objs_per_slab;
        uintptr_t obj_addr = (uintptr_t)obj;
        if (obj_addr >= start && obj_addr < end &&
            ((obj_addr - start) % cache->obj_size == 0)) {
            *(void**)obj = slab->free_list;
            slab->free_list = obj;
            slab->free_count++;
            return;
        }
        slab = slab->next;
    }
    // Możesz dodać obsługę błędu, jeśli obj nie należy do żadnego slab
}