#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

#define IRQ_BASE   0x20
#define IRQ_COUNT  16

typedef void (*irq_handler_t)(void);

// Inicjalizuje system przerwań (remap PIC, ustawia IDT)
void interrupts_init(void);

// Rejestruje handler dla danego numeru IRQ
void irq_register_handler(int irq_num, irq_handler_t handler);

// Handler wyjątków CPU (ISR)
void isr_handler(void);

// Handler sprzętowych przerwań (IRQ)
void irq_handler(int irq_num);

#endif // INTERRUPTS_H