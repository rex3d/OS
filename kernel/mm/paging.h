#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>

#if defined(__x86_64__) || defined(_M_X64)
    #define PAGING_64BIT
    #define PAGE_SIZE 4096
    #define ENTRIES_PER_TABLE 512

    typedef uint64_t page_entry_t;
    typedef uint64_t paging_flags_t;

    // PML4, PDPT, PD, PT mają identyczną strukturę
    typedef struct {
        page_entry_t entries[ENTRIES_PER_TABLE];
    } page_table_t;

    typedef page_table_t page_directory_t; // PML4

#else
    #define PAGING_32BIT
    #define PAGE_SIZE 4096
    #define ENTRIES_PER_TABLE 1024

    typedef uint32_t page_entry_t;
    typedef uint32_t paging_flags_t;

    typedef struct {
        page_entry_t entries[ENTRIES_PER_TABLE];
    } page_table_t;

    typedef page_table_t page_directory_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Inicjalizacja systemu stronicowania
void paging_init(void);

// Mapowanie strony wirtualnej na fizyczną
void paging_map(page_directory_t *dir, uintptr_t virt_addr, uintptr_t phys_addr, paging_flags_t flags);

// Odmapowanie strony wirtualnej
void paging_unmap(page_directory_t *dir, uintptr_t virt_addr);

// Pobranie adresu fizycznego dla danego adresu wirtualnego
uintptr_t paging_get_phys(page_directory_t *dir, uintptr_t virt_addr);

#ifdef __cplusplus
}
#endif

#endif // PAGING_H