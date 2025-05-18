[BITS 16]
section .data
global gdt_start
global gdt_end
global gdt_descriptor

; -------------------------------
; Global Descriptor Table (GDT)
; -------------------------------
gdt_start:
    dq 0x0000000000000000         ; Null descriptor (0x00)
    dq 0x00CF9A000000FFFF         ; Code segment (0x08): base=0, limit=4GB, 32-bit, ring 0
    dq 0x00CF92000000FFFF         ; Data segment (0x10): base=0, limit=4GB, 32-bit, ring 0
gdt_end:

; -------------------------------
; GDT Descriptor (dla instrukcji LGDT)
; -------------------------------
gdt_descriptor:
    dw gdt_end - gdt_start - 1    ; Limit (size - 1)
    dd gdt_start                  ; Base address