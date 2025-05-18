// interrupts.c
#include "interrupts.h"
#include "utils.h"
#include "video.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define IRQ_BASE     0x20
#define IRQ_COUNT    16

typedef void (*irq_handler_t)(void);

extern void isr_stub();  // Assembly ISR stub (CPU exceptions)
extern void irq_stub();  // Assembly IRQ stub (hardware interrupts)

// Tablica wskaźników na funkcje obsługi IRQ
static irq_handler_t irq_handlers[IRQ_COUNT] = {0};

static void remap_pic(void) {
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, IRQ_BASE);      // Master PIC: IRQ0-7 -> INT 32-39
    outb(PIC2_DATA, IRQ_BASE + 8);  // Slave PIC:  IRQ8-15 -> INT 40-47

    outb(PIC1_DATA, 0x04); // Informacja o slave na IRQ2
    outb(PIC2_DATA, 0x02); // Tożsamość slave

    outb(PIC1_DATA, 0x01); // Tryb 8086
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, 0x00); // Odkryj wszystkie IRQ
    outb(PIC2_DATA, 0x00);
}

// Musisz zaimplementować idt_set_gate gdzie indziej
extern void idt_set_gate(int n, uint32_t handler_addr);

void interrupts_init(void) {
    remap_pic();

    // Ustaw stubs dla wszystkich IRQ
    for (int i = 0; i < IRQ_COUNT; ++i) {
        idt_set_gate(IRQ_BASE + i, (uint32_t)irq_stub);
    }
    // Przykład: ISR dla wyjątków CPU (opcjonalnie)
    idt_set_gate(0x00, (uint32_t)isr_stub);

    video_print("Interrupts initialized\n");
}

// Rejestracja własnego handlera dla IRQ
void irq_register_handler(int irq_num, irq_handler_t handler) {
    if (irq_num >= 0 && irq_num < IRQ_COUNT) {
        irq_handlers[irq_num] = handler;
    }
}

// CPU Exception (ISR) handler
void isr_handler(void) {
    video_print("CPU Exception (ISR) caught!\n");
    // TODO: Print more info, e.g., exception number, error code
}

// Hardware Interrupt (IRQ) handler
void irq_handler(int irq_num) {
    // Wywołaj zarejestrowany handler, jeśli istnieje
    if (irq_num >= 0 && irq_num < IRQ_COUNT && irq_handlers[irq_num]) {
        irq_handlers[irq_num]();
    } else {
        char buf[64];
        snprintf(buf, sizeof(buf), "IRQ %d caught\n", irq_num);
        video_print(buf);
    }

    // Wyślij EOI do PIC
    if (irq_num >= 8) {
        outb(PIC2_COMMAND, 0x20);  // EOI do slave PIC
    }
    outb(PIC1_COMMAND, 0x20);      // EOI do master PIC
}
