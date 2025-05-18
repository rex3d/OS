// boot_utils.h - Header file for bootloader utility functions

global print_string
global wait_key
global print_newline
global print_hex

; Function prototypes
; void print_string(char* str)
; Waits for a key press
; void wait_key()
; Prints a newline (CR+LF)
; void print_newline()
; Prints a hexadecimal value in AX
; void print_hex(uint16_t value)