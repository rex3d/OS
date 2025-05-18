#include <stdint.h>
#include "keyboard.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEY_RELEASED_MASK 0x80

#define KEY_BUFFER_SIZE 256

static char key_buffer[KEY_BUFFER_SIZE];
static int head = 0;
static int tail = 0;

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void keyboard_init() {
    // TODO: Podłącz IRQ1 do handlera przerwania
    // Na razie to tylko szkielet
}

static char scancode_to_char(uint8_t scancode) {
    if (scancode & KEY_RELEASED_MASK) return 0; // ignorujemy klawisze puszczone

    switch (scancode) {
        case 0x1E: return 'a';
        case 0x30: return 'b';
        case 0x2E: return 'c';
        case 0x20: return 'd';
        case 0x12: return 'e';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x17: return 'i';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';
        case 0x32: return 'm';
        case 0x31: return 'n';
        case 0x18: return 'o';
        case 0x19: return 'p';
        case 0x10: return 'q';
        case 0x13: return 'r';
        case 0x1F: return 's';
        case 0x14: return 't';
        case 0x16: return 'u';
        case 0x2F: return 'v';
        case 0x11: return 'w';
        case 0x2D: return 'x';
        case 0x15: return 'y';
        case 0x2C: return 'z';
        case 0x39: return ' ';
        case 0x0E: return '\b';
        case 0x1C: return '\n';
        default: return 0;
    }
}

void keyboard_handle_interrupt() {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    char c = scancode_to_char(scancode);

    if (c) {
        int next_head = (head + 1) % KEY_BUFFER_SIZE;

        // zapobiegamy nadpisaniu ogona (czyli taila)
        if (next_head != tail) {
            key_buffer[head] = c;
            head = next_head;
        }
    }
}

char keyboard_get_char() {
    if (tail == head) return 0; // bufor pusty

    char c = key_buffer[tail];
    tail = (tail + 1) % KEY_BUFFER_SIZE;
    return c;
}
