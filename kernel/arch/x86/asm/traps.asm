; traps.asm - Obsługa wyjątków i przerwań (ISR) dla x86 (32/64-bit)
; Wersja uniwersalna: wykrywa tryb kompilacji i generuje odpowiedni kod

%ifdef __x86_64__
    [BITS 64]
    %define PTR_SIZE 8
    %define PUSH_REGS push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11 r12 r13 r14 r15
    %define POP_REGS  pop r15 r14 r13 r12 r11 r10 r9 r8 rbp rdi rsi rdx rcx rbx rax
%else
    [BITS 32]
    %define PTR_SIZE 4
    %define PUSH_REGS pusha
    %define POP_REGS  popa
%endif

section .text

%macro ISR_NOERR 1
global isr%1
isr%1:
    cli
%if PTR_SIZE = 8
    sub rsp, 8                ; align stack for error code
    mov qword [rsp], 0
    mov rdi, %1               ; exception number
    mov rsi, 0                ; error code
    call isr_handler_c
    add rsp, 8
    sti
    iretq
%else
    push dword 0              ; Dummy error code
    push dword %1             ; Exception number
    call isr_handler_c
    add esp, 8
    sti
    iretd
%endif
%endmacro

%macro ISR_ERR 1
global isr%1
isr%1:
    cli
%if PTR_SIZE = 8
    mov rdi, %1               ; exception number
    mov rsi, [rsp+8]          ; error code (already pushed by CPU)
    call isr_handler_c
    sti
    iretq
%else
    push dword %1             ; Exception number
    call isr_handler_c
    add esp, 8
    sti
    iretd
%endif
%endmacro

; 0-7: brak kodu błędu
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7

; 8: Double Fault (#DF) - ma kod błędu
ISR_ERR 8

; 9: brak kodu błędu
ISR_NOERR 9

; 10-14: mają kod błędu
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14

; 15: brak kodu błędu
ISR_NOERR 15

; 16-19: brak kodu błędu
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19

; 20-31: brak kodu błędu
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

extern isr_handler_c