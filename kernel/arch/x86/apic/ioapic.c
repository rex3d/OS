#include "ioapic.h"
#include <stdint.h>

#define IOAPIC_REGSEL   0x00
#define IOAPIC_WINDOW   0x10

static volatile uint32_t* ioapic = (volatile uint32_t*)IOAPIC_BASE;

static uint32_t ioapic_read(uint8_t reg) {
    ioapic[IOAPIC_REGSEL] = reg;
    return ioapic[IOAPIC_WINDOW];
}

static void ioapic_write(uint8_t reg, uint32_t value) {
    ioapic[IOAPIC_REGSEL] = reg;
    ioapic[IOAPIC_WINDOW] = value;
}

uint32_t ioapic_get_id(void) {
    return (ioapic_read(0x00) >> 24) & 0xF;
}

uint32_t ioapic_get_version(void) {
    return ioapic_read(0x01) & 0xFF;
}

uint32_t ioapic_get_max_redir(void) {
    return ((ioapic_read(0x01) >> 16) & 0xFF) + 1;
}

void ioapic_set_entry(int irq, uint64_t data) {
    ioapic_write(0x10 + 2 * irq, (uint32_t)(data & 0xFFFFFFFF));
    ioapic_write(0x10 + 2 * irq + 1, (uint32_t)(data >> 32));
}

uint64_t ioapic_get_entry(int irq) {
    uint32_t low = ioapic_read(0x10 + 2 * irq);
    uint32_t high = ioapic_read(0x10 + 2 * irq + 1);
    return ((uint64_t)high << 32) | low;
}

void ioapic_mask_irq(int irq) {
    uint32_t low = ioapic_read(0x10 + 2 * irq);
    low |= (1 << 16); // set mask bit
    ioapic_write(0x10 + 2 * irq, low);
}

void ioapic_unmask_irq(int irq) {
    uint32_t low = ioapic_read(0x10 + 2 * irq);
    low &= ~(1 << 16); // clear mask bit
    ioapic_write(0x10 + 2 * irq, low);
}

void ioapic_mask_all(void) {
    uint32_t max_irq = ioapic_get_max_redir();
    for (uint32_t i = 0; i < max_irq; ++i) {
        ioapic_mask_irq(i);
    }
}

void ioapic_unmask_all(void) {
    uint32_t max_irq = ioapic_get_max_redir();
    for (uint32_t i = 0; i < max_irq; ++i) {
        ioapic_unmask_irq(i);
    }
}

void ioapic_init(void) {
    uint32_t max_irq = ioapic_get_max_redir();
    // Mask all IRQs and set default entries
    for (uint32_t i = 0; i < max_irq; ++i) {
        // Set default: vector = 0x20 + i, delivery mode = 0, dest = 0
        uint64_t entry = (1ULL << 16) | (0x20 + i); // masked, vector
        ioapic_set_entry(i, entry);
    }
}
