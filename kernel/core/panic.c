#include "panic.h"

#if defined(__x86_64__) || defined(_M_X64)
#include <stdint.h>

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static volatile uint16_t* const vga_buffer = (uint16_t*)VGA_ADDRESS;
static int vga_row = 0;
static int vga_col = 0;
static uint8_t vga_color = 0x4F; // Czerwony na białym tle

static void vga_putc(char c) {
    if (c == '\n') {
        vga_row++;
        vga_col = 0;
    } else {
        vga_buffer[vga_row * VGA_WIDTH + vga_col] = (vga_color << 8) | c;
        vga_col++;
        if (vga_col >= VGA_WIDTH) {
            vga_col = 0;
            vga_row++;
        }
    }
    if (vga_row >= VGA_HEIGHT) {
        vga_row = 0;
    }
}

static void vga_print(const char* msg) {
    while (*msg) {
        vga_putc(*msg++);
    }
}

// Opcjonalnie: wypisywanie na port szeregowy COM1
#define SERIAL_PORT 0x3F8

static int serial_is_transmit_empty() {
    return inb(SERIAL_PORT + 5) & 0x20;
}

static void serial_write(char c) {
    while (!serial_is_transmit_empty());
    outb(SERIAL_PORT, c);
}

static void serial_print(const char* msg) {
    while (*msg) {
        serial_write(*msg++);
    }
}

static void panic_print(const char* msg) {
    vga_print(msg);
    serial_print(msg);
}

#else
#include "video.h"
static void panic_print(const char* msg) {
    video_print(msg);
}
#endif

void panic(const char* message) {
    panic_print("\n*** KERNEL PANIC ***\n");
    panic_print(message);
    panic_print("\nSystem zatrzymany.\n");
    while (1) {
#if defined(__x86_64__) || defined(_M_X64)
        __asm__ volatile ("hlt");
#else
        asm volatile ("hlt");
#endif
    }
}