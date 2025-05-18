#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h> // dla przenośności typów

#ifdef __cplusplus
extern "C" {
#endif

// Typ elementu sterty (możesz zmienić na int64_t, jeśli potrzebujesz większych liczb)
typedef int32_t heap_value_t;

// Struktura sterty (min-heap)
typedef struct {
    heap_value_t *data;   // wskaźnik na tablicę elementów
    size_t capacity;      // maksymalna liczba elementów
    size_t size;          // aktualna liczba elementów
} Heap;

// Tworzy stertę o zadanej pojemności
Heap* heap_create(size_t capacity);

// Zwalnia pamięć sterty
void heap_destroy(Heap *heap);

// Wstawia element do sterty
int heap_insert(Heap *heap, heap_value_t value);

// Usuwa i zwraca najmniejszy element sterty
int heap_extract_min(Heap *heap, heap_value_t *out_value);

// Zwraca najmniejszy element bez usuwania
int heap_peek_min(const Heap *heap, heap_value_t *out_value);

// Sprawdza, czy sterta jest pusta (zwraca 1 jeśli pusta, 0 jeśli nie)
int heap_is_empty(const Heap *heap);

#ifdef __cplusplus
}
#endif

#endif // HEAP_H