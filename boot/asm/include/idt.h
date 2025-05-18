// idt.h - Header file for Interrupt Descriptor Table (IDT) functions and structures

#ifndef IDT_H
#define IDT_H

// Function prototypes
void load_idt();
void set_idt_entry(uint8_t num, uint32_t handler, uint16_t selector, uint8_t type_attr);

// IDT descriptor structure
struct idt_descriptor {
    uint16_t limit;      // Size of the IDT
    uint32_t base;       // Base address of the IDT
};

// Global IDT descriptor
extern struct idt_descriptor idt_desc;

#endif // IDT_H