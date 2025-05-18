; ap_startup.asm - kod startowy dla Application Processor (AP) w SMP
; Ten kod jest ładowany przez BSP do pamięci i uruchamiany przez AP po INIT/SIPI

[BITS 16]
[ORG 0x0000]

ap_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x8000       ; Stos dla AP

    ; Przejście do trybu chronionego
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:ap_pm_start

; ===============================
; ========== PROTECTED ==========
; ===============================
[BITS 32]
ap_pm_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9000

    ; Tutaj AP może zgłosić gotowość do BSP (np. zapis do zmiennej w pamięci)
    ; lub czekać na dalsze instrukcje
.wait:
    hlt
    jmp .wait

; ===============================
; ========== GDT ================
; ===============================
[BITS 16]
gdt_start:
    dq 0x0000000000000000         ; Null
    dq 0x00CF9A000000FFFF         ; Code 0x08
    dq 0x00CF92000000FFFF         ; Data 0x10
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start