#ifndef IOAPIC_H
#define IOAPIC_H

#include <stdint.h>

#define IOAPIC_BASE  0xFEC00000U

// IOAPIC register offsets
#define IOAPIC_REG_ID      0x00
#define IOAPIC_REG_VER     0x01
#define IOAPIC_REG_ARB     0x02
#define IOAPIC_REG_REDTBL  0x10

// Structure for IOAPIC redirection entry (optional, for clarity)
typedef union {
    struct {
        uint32_t low;
        uint32_t high;
    };
    uint64_t value;
} ioapic_redir_entry_t;

void ioapic_init(void);
void ioapic_set_entry(int irq, uint64_t data);
void ioapic_mask_irq(int irq);
void ioapic_unmask_irq(int irq);
uint8_t ioapic_get_max_redir_entries(void);
uint32_t ioapic_read_reg(uint8_t reg);
void ioapic_write_reg(uint8_t reg, uint32_t value);

#endif // IOAPIC_H