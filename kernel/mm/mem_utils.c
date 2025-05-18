#include "mem_utils.h"
#include <stdint.h>

void mem_zero(void *ptr, size_t size) {
    uintptr_t p = (uintptr_t)ptr;
    size_t align = sizeof(uintptr_t);
    unsigned char *byte_ptr = (unsigned char*)ptr;

    // Wyrównanie do słowa
    while (size && (p % align)) {
        *byte_ptr++ = 0;
        --size;
        ++p;
    }

    // Zerowanie słowami
    uintptr_t *word_ptr = (uintptr_t*)byte_ptr;
    size_t word_count = size / align;
    for (size_t i = 0; i < word_count; ++i) {
        word_ptr[i] = 0;
    }

    // Zerowanie pozostałych bajtów
    byte_ptr = (unsigned char*)(word_ptr + word_count);
    size_t rem = size % align;
    for (size_t i = 0; i < rem; ++i) {
        byte_ptr[i] = 0;
    }
}

void mem_copy(void *dest, const void *src, size_t size) {
    uintptr_t d = (uintptr_t)dest;
    uintptr_t s = (uintptr_t)src;
    size_t align = sizeof(uintptr_t);

    unsigned char *d_byte = (unsigned char*)dest;
    const unsigned char *s_byte = (const unsigned char*)src;

    // Wyrównanie do słowa
    while (size && ((d % align) || (s % align))) {
        *d_byte++ = *s_byte++;
        --size;
        ++d;
        ++s;
    }

    // Kopiowanie słowami
    uintptr_t *d_word = (uintptr_t*)d_byte;
    const uintptr_t *s_word = (const uintptr_t*)s_byte;
    size_t word_count = size / align;
    for (size_t i = 0; i < word_count; ++i) {
        d_word[i] = s_word[i];
    }

    // Kopiowanie pozostałych bajtów
    d_byte = (unsigned char*)(d_word + word_count);
    s_byte = (const unsigned char*)(s_word + word_count);
    size_t rem = size % align;
    for (size_t i = 0; i < rem; ++i) {
        d_byte[i] = s_byte[i];
    }
}

int mem_compare(const void *a, const void *b, size_t size) {
    uintptr_t pa = (uintptr_t)a;
    uintptr_t pb = (uintptr_t)b;
    size_t align = sizeof(uintptr_t);

    const unsigned char *a_byte = (const unsigned char*)a;
    const unsigned char *b_byte = (const unsigned char*)b;

    // Porównanie do wyrównania
    while (size && ((pa % align) || (pb % align))) {
        if (*a_byte != *b_byte)
            return (int)*a_byte - (int)*b_byte;
        ++a_byte;
        ++b_byte;
        --size;
        ++pa;
        ++pb;
    }

    // Porównanie słowami
    const uintptr_t *a_word = (const uintptr_t*)a_byte;
    const uintptr_t *b_word = (const uintptr_t*)b_byte;
    size_t word_count = size / align;
    for (size_t i = 0; i < word_count; ++i) {
        if (a_word[i] != b_word[i]) {
            // Rozbij na bajty, żeby znaleźć różnicę
            const unsigned char *aw = (const unsigned char*)&a_word[i];
            const unsigned char *bw = (const unsigned char*)&b_word[i];
            for (size_t j = 0; j < align; ++j) {
                if (aw[j] != bw[j])
                    return (int)aw[j] - (int)bw[j];
            }
        }
    }

    // Porównanie pozostałych bajtów
    a_byte = (const unsigned char*)(a_word + word_count);
    b_byte = (const unsigned char*)(b_word + word_count);
    size_t rem = size % align;
    for (size_t i = 0; i < rem; ++i) {
        if (a_byte[i] != b_byte[i])
            return (int)a_byte[i] - (int)b_byte[i];
    }
    return 0;
}