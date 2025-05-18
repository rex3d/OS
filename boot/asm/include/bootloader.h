// bootloader.h - Header file for the bootloader functions and entry points

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

extern void start(void);
extern void protected_mode_entry(void);
extern void print_string_pm(const char* str);
extern void wait_key(void);
extern void print_newline_pm(void);

#endif // BOOTLOADER_H