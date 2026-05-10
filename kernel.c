#include <stddef.h>

void execute_command();
char get_key();
unsigned char inb(unsigned short port);
void scroll();

volatile char* video = (volatile char*)0xB8000;
int cursor = 0;

char scancode_table[128] = {
0,27,'1','2','3','4','5','6','7','8','9','0','-','=',8,9,
'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s',
'd','f','g','h','j','k','l',';',39,'`',0,'\\','z','x','c','v',
'b','n','m',',','.','/',0,'*',0,' '
};

char input[256];
int input_pos = 0;

void print(const char* str) {
    while (*str) {
        video[cursor++] = *str++;
        video[cursor++] = 0x07;

        if (cursor >= 80 * 25 * 2) {
            scroll();
        }
    }
}

void newline() {

    int line = cursor / 160;
    cursor = (line + 1) * 160;

    if (cursor >= 80 * 25 * 2) {
        scroll();
    }
}

void clear() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video[i] = ' ';
        video[i+1] = 0x07;
    }
    cursor = 0;
}

void scroll() {

    for (int i = 0; i < 24 * 80 * 2; i++) {
        video[i] = video[i + 160];
    }

    for (int i = 24 * 160; i < 25 * 160; i += 2) {
        video[i] = ' ';
        video[i+1] = 0x07;
    }

    cursor -= 160;
}

void kernel_main() {

    clear();
    print("vnOS v0.1");
    newline();
    print(">");

    while (1) {

        char c = get_key();

        if (c == '\n') {

            input[input_pos] = 0;
            execute_command();

        } else if (c == 8 && input_pos > 0) {   // backspace

            input_pos--;
            cursor -= 2;

            video[cursor] = ' ';
            video[cursor + 1] = 0x07;
        }

        else if (c > 0) {

            if (input_pos < 255) {   // prevent overflow
                input[input_pos++] = c;
            }

            video[cursor++] = c;
            video[cursor++] = 0x07;
        }
    }
}

unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

char get_key() {
    unsigned char scancode;

    while (1) {
        if (inb(0x64) & 1) {
            scancode = inb(0x60);

            if (scancode < 128) {
                return scancode_table[scancode];
            }
        }
    }
}

int strcmp(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return (*a == 0 && *b == 0);
}

void execute_command() {

    if (strcmp(input, "help")) {
        newline();
        print("commands: help clear ver");
    }

    else if (strcmp(input, "ver")) {
        newline();
        print("vnOS version 0.1");
    }

    else if (strcmp(input, "clear")) {
        clear();
    }

    else {
        newline();
        print("Unknown command");
    }

    input_pos = 0;
    input[0] = 0;

    newline();
    print(">");
}

