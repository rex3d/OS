section .multiboot
    align 4

    MB_HEADER_MAGIC    equ 0x1BADB002
    MB_HEADER_FLAGS    equ 0x00000003
    MB_HEADER_CHECKSUM equ -(MB_HEADER_MAGIC + MB_HEADER_FLAGS)

    dd MB_HEADER_MAGIC
    dd MB_HEADER_FLAGS
    dd MB_HEADER_CHECKSUM

[BITS 16]
[ORG 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov si, msg1
    call print_string
    call wait_key

    ; Załaduj GDT
    lgdt [gdt_descriptor]

    ; Włącz protected mode (bit PE w CR0)
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Skok daleki do kodu 32-bitowego
    jmp 0x08:protected_mode_entry

[BITS 32]
protected_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    mov esi, msg2
    call print_string_pm

    jmp $

; 16-bit print string
print_string:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

; wait for key press
wait_key:
    xor ah, ah
    int 0x16
    ret

; 32-bit print string
print_string_pm:
    lodsb
    test al, al
    jz .done_pm
    mov ah, 0x0E
    mov bx, 0x0007
    int 0x10
    jmp print_string_pm
.done_pm:
    ret

[BITS 16]
; GDT (3 segmenty po 8 bajtów każdy, total 24 bajty)
gdt_start:
    dq 0x0000000000000000     ; null descriptor
    dq 0x00CF9A000000FFFF     ; code segment descriptor
    dq 0x00CF92000000FFFF     ; data segment descriptor

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt_end:

msg1 db 'BOOTLOADER DZIALA! Nacisnij klawisz...', 0
msg2 db 'Jestes w protected mode!', 0

times 510 - ($ - $$) db 0
dw 0xAA55
