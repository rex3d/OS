[BITS 16]
global enable_long_mode

section .text

enable_long_mode:
    ; Włącz A20 (żeby mieć dostęp do >1MB pamięci)
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Załaduj GDT 64-bitowy
    lgdt [gdt64_descriptor]

    ; Ustaw segmenty danych 16-bit (potrzebne przed przejściem w protected mode)
    mov ax, 0x10        ; selector data segment (offset 0x10 w GDT)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Włącz protected mode (PE bit w CR0)
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Włącz PAE (bit 5 w CR4)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Załaduj adres tabeli stron do CR3
    mov eax, pml4_table
    mov cr3, eax

    ; Włącz LME (Long Mode Enable) w IA32_EFER MSR (0xC0000080)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8       ; ustaw bit LME
    mov edx, 0           ; wyczyść EDX, żeby MSR był poprawny
    wrmsr

    ; Włącz paging i protected mode (bit 31 i 0 w CR0)
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax

    ; Skok daleki do kodu 64-bitowego (selector 0x28 w GDT, long_mode_start)
    jmp 0x28:long_mode_start

; -------------------------------------------------------------------

[BITS 64]

long_mode_start:
    ; Załaduj segment kodu 64-bitowego (już ustawione przez skok daleki)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Prosta funkcja do wypisania stringa przez BIOS int 0x10 (dla testu)
    mov rsi, hello_message
    call print_string64

    ; Zatrzymaj się w nieskończonej pętli
    cli
.hlt_loop:
    hlt
    jmp .hlt_loop

; -------------------------------------------------------------------

print_string64:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string64
.done:
    ret

; -------------------------------------------------------------------

section .data
align 8
hello_message db "Hello from 64-bit long mode, kurwa!", 0

align 8
gdt64_start:
    dq 0x0000000000000000        ; null descriptor
    dq 0x00AF9A000000FFFF        ; 64-bit code segment (base 0, limit max, exec, readable)
    dq 0x00AF92000000FFFF        ; 64-bit data segment (base 0, limit max, writable)
gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64_start - 1
    dq gdt64_start

align 4096
pml4_table:
    dq pdpt_table | 0x03          ; Present + RW
    times 511 dq 0

align 4096
pdpt_table:
    dq pd_table | 0x03            ; Present + RW
    times 511 dq 0

align 4096
pd_table:
    dq 0x00000000 | 0x83          ; Present + RW + Page size (2MB)
    times 511 dq 0
