; interrupts.asm - Obsługa przerwań sprzętowych (IRQ) dla x86 (32/64-bit)
; Uniwersalny kod dla 32- i 64-bit, zgodny z ABI System V (64-bit) i cdecl (32-bit)

%ifdef __x86_64__
    [BITS 64]
    %define PTR_SIZE 8
    %define IRQ_REGS push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11 r12 r13 r14 r15
    %define IRQ_REGS_POP pop r15 r14 r13 r12 r11 r10 r9 r8 rbp rdi rsi rdx rcx rbx rax
%else
    [BITS 32]
    %define PTR_SIZE 4
    %define IRQ_REGS pusha
    %define IRQ_REGS_POP popa
%endif

section .text

%macro IRQ_HANDLER 1
global irq%1
irq%1:
    cli
%if PTR_SIZE = 8
    IRQ_REGS
    mov rdi, %1           ; numer IRQ jako pierwszy argument
    call irq_handler_c
    IRQ_REGS_POP
    sti
    iretq
%else
    push dword %1         ; numer IRQ jako argument
    call irq_handler_c
    add esp, 4
    sti
    iretd
%endif
%endmacro

; IRQ0-15 (timer, klawiatura, ... kontrolery PIC)
IRQ_HANDLER 0
IRQ_HANDLER 1
IRQ_HANDLER 2
IRQ_HANDLER 3
IRQ_HANDLER 4
IRQ_HANDLER 5
IRQ_HANDLER 6
IRQ_HANDLER 7
IRQ_HANDLER 8
IRQ_HANDLER 9
IRQ_HANDLER 10
IRQ_HANDLER 11
IRQ_HANDLER 12
IRQ_HANDLER 13
IRQ_HANDLER 14
IRQ_HANDLER 15

extern irq_handler_c