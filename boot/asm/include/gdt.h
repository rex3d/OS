// gdt.h - Header file for Global Descriptor Table (GDT) functions and structures

#ifndef GDT_H
#define GDT_H

// GDT entry structure
typedef struct {
    uint16_t limit_low;    // Lower 16 bits of the limit
    uint16_t base_low;     // Lower 16 bits of the base
    uint8_t  base_middle;   // Next 8 bits of the base
    uint8_t  access;        // Access flags
    uint8_t  granularity;   // Granularity and upper 4 bits of the limit
    uint8_t  base_high;     // Last 8 bits of the base
} __attribute__((packed)) gdt_entry_t;

// GDT pointer structure
typedef struct {
    uint16_t limit;         // Size of the GDT
    uint32_t base;          // Address of the GDT
} __attribute__((packed)) gdt_ptr_t;

// Function declarations
void gdt_install();
void gdt_set_gate(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

#endif // GDT_H