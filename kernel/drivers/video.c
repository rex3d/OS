#include "video.h"
#include "utils.h"  // outb()

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR 0x07  // szary na czarnym tle

static uint16_t* const VGA_BUFFER = (uint16_t*)VGA_ADDRESS;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static void move_cursor() {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static void scroll_screen() {
    for (int y = 0; y < VGA_HEIGHT - 1; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = VGA_BUFFER[(y + 1) * VGA_WIDTH + x];
        }
    }
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t)(' ' | (VGA_COLOR << 8));
    }
    if (cursor_y > 0) cursor_y--;
}

void video_clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_BUFFER[i] = (uint16_t)(' ' | (VGA_COLOR << 8));
    }
    cursor_x = 0;
    cursor_y = 0;
    move_cursor();
}

void video_clear() {
    video_clear_screen();
}

void video_print_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\t') {
        int spaces = 4 - (cursor_x % 4);
        for (int i = 0; i < spaces; i++) {
            video_print_char(' ');
        }
        return;
    } else {
        VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = (uint16_t)(c | (VGA_COLOR << 8));
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= VGA_HEIGHT) {
        scroll_screen();
    }

    move_cursor();
}

void video_print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        video_print_char(str[i]);
    }
}
