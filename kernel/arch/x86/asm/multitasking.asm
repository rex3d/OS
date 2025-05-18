; multitasking.asm - przełączanie kontekstu dla x86 (32/64-bit)

section .text

; ===============================
; ======== 32-bit x86 ===========
; ===============================
global switch_task32_asm

; void switch_task32_asm(uint32_t* old_esp, uint32_t* old_ebp, uint32_t new_esp, uint32_t new_ebp)
; [esp+4]  = old_esp (wskaźnik)
; [esp+8]  = old_ebp (wskaźnik)
; [esp+12] = new_esp (wartość)
; [esp+16] = new_ebp (wartość)

switch_task32_asm:
    push ebp
    mov ebp, esp

    mov eax, [ebp+8]      ; old_esp
    mov [eax], esp        ; zapisujemy aktualny esp

    mov eax, [ebp+12]     ; old_ebp
    mov [eax], ebp        ; zapisujemy aktualny ebp

    mov esp, [ebp+16]     ; ustawiamy nowy esp
    mov ebp, [ebp+20]     ; ustawiamy nowy ebp

    pop ebp
    ret

; ===============================
; ======== 64-bit x86_64 ========
; ===============================
global switch_task64_asm

; void switch_task64_asm(uint64_t* old_rsp, uint64_t* old_rbp, uint64_t new_rsp, uint64_t new_rbp)
; rdi = old_rsp (wskaźnik)
; rsi = old_rbp (wskaźnik)
; rdx = new_rsp (wartość)
; rcx = new_rbp (wartość)

switch_task64_asm:
    mov [rdi], rsp        ; zapisujemy aktualny rsp
    mov [rsi], rbp        ; zapisujemy aktualny rbp
    mov rsp, rdx          ; ustawiamy nowy rsp
    mov rbp, rcx          ; ustawiamy nowy rbp
    ret