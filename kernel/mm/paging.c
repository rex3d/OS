#include "paging.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(PAGING_64BIT)

static page_directory_t kernel_pml4;

void paging_init(void) {
    memset(&kernel_pml4, 0, sizeof(kernel_pml4));
}

void paging_map(page_directory_t *pml4, uintptr_t virt_addr, uintptr_t phys_addr, uint64_t flags) {
    size_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    size_t pdpt_idx = (virt_addr >> 30) & 0x1FF;
    size_t pd_idx   = (virt_addr >> 21) & 0x1FF;
    size_t pt_idx   = (virt_addr >> 12) & 0x1FF;

    page_table_t *pdpt, *pd, *pt;

    if (!(pml4->entries[pml4_idx] & 0x1)) {
        pdpt = (page_table_t*)calloc(1, sizeof(page_table_t));
        pml4->entries[pml4_idx] = ((uintptr_t)pdpt) | (flags & 0xFFF) | 0x1;
    } else {
        pdpt = (page_table_t*)(pml4->entries[pml4_idx] & ~0xFFFULL);
    }

    if (!(pdpt->entries[pdpt_idx] & 0x1)) {
        pd = (page_table_t*)calloc(1, sizeof(page_table_t));
        pdpt->entries[pdpt_idx] = ((uintptr_t)pd) | (flags & 0xFFF) | 0x1;
    } else {
        pd = (page_table_t*)(pdpt->entries[pdpt_idx] & ~0xFFFULL);
    }

    if (!(pd->entries[pd_idx] & 0x1)) {
        pt = (page_table_t*)calloc(1, sizeof(page_table_t));
        pd->entries[pd_idx] = ((uintptr_t)pt) | (flags & 0xFFF) | 0x1;
    } else {
        pt = (page_table_t*)(pd->entries[pd_idx] & ~0xFFFULL);
    }

    pt->entries[pt_idx] = (phys_addr & ~0xFFFULL) | (flags & 0xFFF) | 0x1;
}

void paging_unmap(page_directory_t *pml4, uintptr_t virt_addr) {
    size_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    size_t pdpt_idx = (virt_addr >> 30) & 0x1FF;
    size_t pd_idx   = (virt_addr >> 21) & 0x1FF;
    size_t pt_idx   = (virt_addr >> 12) & 0x1FF;

    page_table_t *pdpt = (page_table_t*)(pml4->entries[pml4_idx] & ~0xFFFULL);
    if (!pdpt) return;
    page_table_t *pd = (page_table_t*)(pdpt->entries[pdpt_idx] & ~0xFFFULL);
    if (!pd) return;
    page_table_t *pt = (page_table_t*)(pd->entries[pd_idx] & ~0xFFFULL);
    if (!pt) return;

    pt->entries[pt_idx] = 0;
}

uintptr_t paging_get_phys(page_directory_t *pml4, uintptr_t virt_addr) {
    size_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    size_t pdpt_idx = (virt_addr >> 30) & 0x1FF;
    size_t pd_idx   = (virt_addr >> 21) & 0x1FF;
    size_t pt_idx   = (virt_addr >> 12) & 0x1FF;

    page_table_t *pdpt = (page_table_t*)(pml4->entries[pml4_idx] & ~0xFFFULL);
    if (!pdpt) return 0;
    page_table_t *pd = (page_table_t*)(pdpt->entries[pdpt_idx] & ~0xFFFULL);
    if (!pd) return 0;
    page_table_t *pt = (page_table_t*)(pd->entries[pd_idx] & ~0xFFFULL);
    if (!pt) return 0;

    if (!(pt->entries[pt_idx] & 0x1)) return 0;
    return (pt->entries[pt_idx] & ~0xFFFULL) | (virt_addr & 0xFFF);
}

page_directory_t *paging_get_kernel_directory(void) {
    return &kernel_pml4;
}

#else // 32-bit

static page_directory_t kernel_page_directory;

void paging_init(void) {
    memset(&kernel_page_directory, 0, sizeof(kernel_page_directory));
}

void paging_map(page_directory_t *dir, uintptr_t virt_addr, uintptr_t phys_addr, uint64_t flags) {
    size_t dir_idx = (virt_addr >> 22) & 0x3FF;
    size_t tbl_idx = (virt_addr >> 12) & 0x3FF;

    page_table_t *table;
    if (!(dir->entries[dir_idx] & 0x1)) {
        table = (page_table_t*)calloc(1, sizeof(page_table_t));
        dir->entries[dir_idx] = ((uintptr_t)table) | ((uint32_t)flags & 0xFFF) | 0x1;
    } else {
        table = (page_table_t*)(dir->entries[dir_idx] & ~0xFFF);
    }

    table->entries[tbl_idx] = (phys_addr & ~0xFFF) | ((uint32_t)flags & 0xFFF) | 0x1;
}

void paging_unmap(page_directory_t *dir, uintptr_t virt_addr) {
    size_t dir_idx = (virt_addr >> 22) & 0x3FF;
    size_t tbl_idx = (virt_addr >> 12) & 0x3FF;

    if (!(dir->entries[dir_idx] & 0x1)) return;

    page_table_t *table = (page_table_t*)(dir->entries[dir_idx] & ~0xFFF);
    table->entries[tbl_idx] = 0;
}

uintptr_t paging_get_phys(page_directory_t *dir, uintptr_t virt_addr) {
    size_t dir_idx = (virt_addr >> 22) & 0x3FF;
    size_t tbl_idx = (virt_addr >> 12) & 0x3FF;

    if (!(dir->entries[dir_idx] & 0x1)) return 0;

    page_table_t *table = (page_table_t*)(dir->entries[dir_idx] & ~0xFFF);
    if (!(table->entries[tbl_idx] & 0x1)) return 0;

    return (table->entries[tbl_idx] & ~0xFFF) | (virt_addr & 0xFFF);
}

page_directory_t *paging_get_kernel_directory(void) {
    return &kernel_page_directory;
}

#endif