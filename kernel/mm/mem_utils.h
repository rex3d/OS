#ifndef MEM_UTILS_H
#define MEM_UTILS_H

#include <stddef.h>
#include <stdint.h> // zapewnia przenośność typów

#ifdef __cplusplus
extern "C" {
#endif

// Ustawia blok pamięci na zero
void mem_zero(void *ptr, size_t size);

// Kopiuje blok pamięci
void mem_copy(void *dest, const void *src, size_t size);

// Porównuje dwa bloki pamięci
int mem_compare(const void *a, const void *b, size_t size);

#ifdef __cplusplus
}
#endif

#endif // MEM_UTILS_H