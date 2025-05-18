[BITS 16]
global print_string
global wait_key
global print_newline
global print_hex

; Wyświetla napis zakończony znakiem 0 lub '$'
print_string:
    pusha
.next_char:
    lodsb
    cmp al, 0
    je .done
    cmp al, '$'
    je .done
    mov ah, 0x0E
    int 0x10
    jmp .next_char
.done:
    popa
    ret

; Czeka na naciśnięcie klawisza
wait_key:
    mov ah, 0
    int 0x16
    ret

; Wyświetla nową linię (CR+LF)
print_newline:
    pusha
    mov ah, 0x0E
    mov al, 0x0D
    int 0x10
    mov al, 0x0A
    int 0x10
    popa
    ret

; Wyświetla AX jako 4 cyfry szesnastkowe
print_hex:
    pusha
    mov cx, 4
    mov bx, ax
.hex_loop:
    rol bx, 4
    mov al, bl
    and al, 0x0F
    cmp al, 9
    jbe .num
    add al, 'A' - 10
    jmp .out
.num:
    add al, '0'
.out:
    mov ah, 0x0E
    int 0x10
    loop .hex_loop
    popa
    ret