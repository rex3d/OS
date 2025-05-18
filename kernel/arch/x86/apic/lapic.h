#ifndef LAPIC_H
#define LAPIC_H

#include <stdint.h>

#define LAPIC_BASE        0xFEE00000U

// LAPIC register offsets
#define LAPIC_ID          0x020
#define LAPIC_EOI         0x0B0
#define LAPIC_SVR         0x0F0
#define LAPIC_ICR_LOW     0x300
#define LAPIC_ICR_HIGH    0x310

// LAPIC Spurious Interrupt Vector Register flags
#define LAPIC_ENABLE      0x100

// Function prototypes
void lapic_init(void);
void lapic_eoi(void);
void lapic_send_ipi(uint8_t apic_id, uint32_t vector);
uint32_t lapic_read(uint32_t reg);
void lapic_write(uint32_t reg, uint32_t value);

#endif // LAPIC_H