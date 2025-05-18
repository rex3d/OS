#include "lapic.h"
#include <stdint.h>

#define LAPIC_REG(offset) (*(volatile uint32_t*)((uintptr_t)LAPIC_BASE + (offset)))

#define LAPIC_ID         0x20
#define LAPIC_VER        0x30
#define LAPIC_TPR        0x80
#define LAPIC_EOI        0xB0
#define LAPIC_SVR        0xF0
#define LAPIC_ICR_LOW    0x300
#define LAPIC_ICR_HIGH   0x310
#define LAPIC_LVT_TIMER  0x320
#define LAPIC_TIMER_INIT 0x380
#define LAPIC_TIMER_CURR 0x390
#define LAPIC_TIMER_DIV  0x3E0

#define LAPIC_ENABLE     0x100

// Ustaw domyślną bazę LAPIC, jeśli nie zdefiniowano
#ifndef LAPIC_BASE
#define LAPIC_BASE 0xFEE00000UL
#endif

static inline void lapic_write(uint32_t reg, uint32_t value) {
    LAPIC_REG(reg) = value;
}

static inline uint32_t lapic_read(uint32_t reg) {
    return LAPIC_REG(reg);
}

void lapic_init(void) {
    // Włącz LAPIC (ustaw bit enable w Spurious Interrupt Vector Register)
    lapic_write(LAPIC_SVR, lapic_read(LAPIC_SVR) | LAPIC_ENABLE);

    // Opcjonalnie: ustaw priorytet, wyczyść EOI, skonfiguruj timer itp.
    lapic_write(LAPIC_TPR, 0x00); // Zezwól na wszystkie przerwania
    lapic_eoi();

    // Przykład: wyłącz timer LAPIC (możesz skonfigurować według potrzeb)
    lapic_write(LAPIC_LVT_TIMER, 1 << 16); // Mask timer interrupt
}

void lapic_eoi(void) {
    lapic_write(LAPIC_EOI, 0);
}

void lapic_send_ipi(uint8_t apic_id, uint32_t vector) {
    // Ustaw docelowy APIC ID
    lapic_write(LAPIC_ICR_HIGH, ((uint32_t)apic_id) << 24);
    // Wyślij IPI (tylko fixed delivery mode, poziom=0, trigger=0)
    lapic_write(LAPIC_ICR_LOW, vector & 0xFF);
}