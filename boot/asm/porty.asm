global outb
global inb
global outw
global inw
global outl
global inl
global outb64
global inb64
global outw64
global inw64
global outl64
global inl64

section .text

; --- 32-bitowe wersje (argumenty na stosie) ---

; void outb(uint16_t port, uint8_t val)
outb:
    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al
    ret

; uint8_t inb(uint16_t port)
inb:
    mov dx, [esp + 4]
    in al, dx
    movzx eax, al
    ret

; void outw(uint16_t port, uint16_t val)
outw:
    mov dx, [esp + 4]
    mov ax, [esp + 8]
    out dx, ax
    ret

; uint16_t inw(uint16_t port)
inw:
    mov dx, [esp + 4]
    in ax, dx
    movzx eax, ax
    ret

; void outl(uint16_t port, uint32_t val)
outl:
    mov dx, [esp + 4]
    mov eax, [esp + 8]
    out dx, eax
    ret

; uint32_t inl(uint16_t port)
inl:
    mov dx, [esp + 4]
    in eax, dx
    ret

; --- 64-bitowe wersje (argumenty w rejestrach, System V AMD64 ABI) ---

; void outb64(uint16_t port, uint8_t val)
; port -> di, val -> sil
outb64:
    mov dx, di
    mov al, sil
    out dx, al
    ret

; uint8_t inb64(uint16_t port)
; port -> di
inb64:
    mov dx, di
    in al, dx
    movzx eax, al
    ret

; void outw64(uint16_t port, uint16_t val)
; port -> di, val -> si
outw64:
    mov dx, di
    mov ax, si
    out dx, ax
    ret

; uint16_t inw64(uint16_t port)
; port -> di
inw64:
    mov dx, di
    in ax, dx
    movzx eax, ax
    ret

; void outl64(uint16_t port, uint32_t val)
; port -> di, val -> esi
outl64:
    mov dx, di
    mov eax, esi
    out dx, eax
    ret

; uint32_t inl64(uint16_t port)
; port -> di
inl64:
    mov dx, di
    in eax, dx
    ret