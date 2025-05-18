#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#ifdef __cplusplus
extern "C" {
#endif

// IRQ handler declarations (hardware interrupts 0-15)
void irq0(void);
void irq1(void);
void irq2(void);
void irq3(void);
void irq4(void);
void irq5(void);
void irq6(void);
void irq7(void);
void irq8(void);
void irq9(void);
void irq10(void);
void irq11(void);
void irq12(void);
void irq13(void);
void irq14(void);
void irq15(void);

// Generic IRQ handler in C
void irq_handler_c(int irq);

// Optional: Array of IRQ handlers for easier management
typedef void (*irq_handler_t)(void);
extern irq_handler_t irq_handlers[16];

#ifdef __cplusplus
}
#endif

#endif // INTERRUPTS_H