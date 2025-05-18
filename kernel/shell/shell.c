#include "../drivers/keyboard.h"
#include "../drivers/video.h"
#include "shell.h"
#include <string.h>

#define INPUT_BUFFER_SIZE 256

static char input_buffer[INPUT_BUFFER_SIZE];

void shell_init() {
    clear_screen();
    print("RexOS Terminal v0.1\n> ");
}

void execute_command(const char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        print("Available commands:\n");
        print(" - help\n - clear\n - echo <text>\n - halt\n");
    } else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        print(cmd + 5);
        print("\n");
    } else if (strcmp(cmd, "halt") == 0) {
        print("System halt.\n");
        __asm__ __volatile__("cli; hlt");
    } else {
        print("Unknown command. Type 'help'.\n");
    }
}

void shell_run() {
    int index = 0;
    while (1) {
        char c = get_char();  // Musi być funkcja z keyboarda

        if (c == '\n') {
            input_buffer[index] = '\0';
            print("\n");
            execute_command(input_buffer);
            print("> ");
            index = 0;
        } else if (c == '\b') {
            if (index > 0) {
                index--;
                backspace();  // Musi być w screen.c
            }
        } else if (index < INPUT_BUFFER_SIZE - 1) {
            input_buffer[index++] = c;
            char str[2] = {c, 0};
            print(str);
        }
    }
}
