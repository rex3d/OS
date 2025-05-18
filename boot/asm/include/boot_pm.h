// boot_pm.h - Header file for protected mode operations in the bootloader

#ifndef BOOT_PM_H
#define BOOT_PM_H

// Function declarations for protected mode operations
void setup_gdt();
void switch_to_protected_mode();
void print_string_pm(const char* str);
void print_newline_pm();

#endif // BOOT_PM_H