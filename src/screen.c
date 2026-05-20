#include "screen.h"
#include "io.h"

volatile char* video = (volatile char*)0xB8000;
int cursor = 0;

void move_cursor() {

    unsigned short pos = cursor / 2;

    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);

    outb(0x3D4, 15);
    outb(0x3D5, pos);
}

void scroll() {

    for (int i = 0; i < 24 * 80 * 2; i++) {
        video[i] = video[i + 160];
    }

    for (int i = 24 * 160; i < 25 * 160; i += 2) {

        video[i] = ' ';
        video[i + 1] = 0x07;
    }

    cursor -= 160;

    move_cursor();
}

void put_char(char c) {

    video[cursor++] = c;
    video[cursor++] = 0x07;

    move_cursor();

    if (cursor >= 80 * 25 * 2) {
        scroll();
    }
}

void backspace() {

    cursor -= 2;

    video[cursor] = ' ';
    video[cursor + 1] = 0x07;

    move_cursor();
}

void print(const char* str) {

    while (*str) {
        put_char(*str++);
    }
}

void print_int(int num) {

    char buffer[16];
    int i = 0;

    if (num == 0) {

        print("0");
        return;
    }

    if (num < 0) {

        print("-");
        num = -num;
    }

    while (num > 0) {

        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i > 0) {

        char c[2];

        c[0] = buffer[--i];
        c[1] = 0;

        print(c);
    }
}

void newline() {

    int line = cursor / 160;

    cursor = (line + 1) * 160;

    if (cursor >= 80 * 25 * 2) {
        scroll();
    }

    move_cursor();
}

void clear() {

    for (int i = 0; i < 80 * 25 * 2; i += 2) {

        video[i] = ' ';
        video[i + 1] = 0x07;
    }

    cursor = 0;

    move_cursor();
}
