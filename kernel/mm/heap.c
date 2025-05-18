#include "heap.h"
#include <stdlib.h>
#include <stdint.h> // dla intptr_t

typedef struct Heap {
    intptr_t *data;
    size_t capacity;
    size_t size;
} Heap;

static void heapify_up(Heap *heap, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (heap->data[parent] <= heap->data[idx]) break;
        intptr_t tmp = heap->data[parent];
        heap->data[parent] = heap->data[idx];
        heap->data[idx] = tmp;
        idx = parent;
    }
}

static void heapify_down(Heap *heap, size_t idx) {
    while (2 * idx + 1 < heap->size) {
        size_t left = 2 * idx + 1;
        size_t right = 2 * idx + 2;
        size_t smallest = idx;

        if (heap->data[left] < heap->data[smallest]) smallest = left;
        if (right < heap->size && heap->data[right] < heap->data[smallest]) smallest = right;
        if (smallest == idx) break;

        intptr_t tmp = heap->data[idx];
        heap->data[idx] = heap->data[smallest];
        heap->data[smallest] = tmp;
        idx = smallest;
    }
}

Heap* heap_create(size_t capacity) {
    if (capacity == 0) return NULL;
    Heap *heap = (Heap*)malloc(sizeof(Heap));
    if (!heap) return NULL;
    heap->data = (intptr_t*)malloc(capacity * sizeof(intptr_t));
    if (!heap->data) {
        free(heap);
        return NULL;
    }
    heap->capacity = capacity;
    heap->size = 0;
    return heap;
}

void heap_destroy(Heap *heap) {
    if (heap) {
        free(heap->data);
        free(heap);
    }
}

int heap_insert(Heap *heap, intptr_t value) {
    if (!heap || heap->size >= heap->capacity) return 0;
    heap->data[heap->size] = value;
    heapify_up(heap, heap->size);
    heap->size++;
    return 1;
}

intptr_t heap_extract_min(Heap *heap) {
    if (!heap || heap->size == 0) return (intptr_t)0;
    intptr_t min = heap->data[0];
    heap->data[0] = heap->data[--heap->size];
    heapify_down(heap, 0);
    return min;
}

intptr_t heap_peek_min(const Heap *heap) {
    if (!heap || heap->size == 0) return (intptr_t)0;
    return heap->data[0];
}

int heap_is_empty(const Heap *heap) {
    return (!heap || heap->size == 0);
}