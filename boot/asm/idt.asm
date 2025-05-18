; idt.asm - Prosta inicjalizacja IDT dla x86 (16/32-bit)
; Tworzy pustą tablicę IDT oraz przykładowy handler przerwania (np. dla #UD)

[BITS 32]
section .data
align 8

idt_start:
    times 256 dq 0          ; 256 pustych deskryptorów (8 bajtów każdy)
idt_end:

idt_descriptor:
    dw idt_end - idt_start - 1
    dd idt_start

section .text
global load_idt
global set_idt_entry

; void load_idt()
load_idt:
    lidt [idt_descriptor]
    ret

; void set_idt_entry(uint8_t num, uint32_t handler, uint16_t selector, uint8_t type_attr)
; Argumenty: num (na stosie+4), handler (na stosie+8), selector (na stosie+12), type_attr (na stosie+14)
set_idt_entry:
    push ebp
    mov ebp, esp

    mov ecx, [ebp+8]    ; handler
    mov edx, [ebp+12]   ; selector
    mov bl, [ebp+14]    ; type_attr
    mov eax, [ebp+4]    ; num

    mov edi, idt_start
    movzx eax, al
    imul eax, 8         ; 8 bajtów na wpis
    add edi, eax

    mov word [edi], cx          ; offset 0..15
    mov word [edi+2], dx        ; selector
    mov byte [edi+4], 0         ; zero
    mov byte [edi+5], bl        ; type_attr
    shr ecx, 16
    mov word [edi+6], cx        ; offset 16..31

    pop ebp
    ret

; Przykładowy handler przerwania (#UD - Invalid Opcode)
global isr_ud
isr_ud:
    pusha
    mov al, '!'
    mov ah, 0x0E
    int 0x10
    popa
    iret

; --- Przykład inicjalizacji wpisu dla #UD (opcjonalnie, do wywołania z kodu C lub bootloadera) ---
; call set_idt_entry, 6, isr_ud, 0x08, 0x8E