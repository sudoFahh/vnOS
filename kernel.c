#include <stddef.h>

/* =========================
   FUNCTION DECLARATIONS
========================= */

void execute_basic();
char get_key();

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char val);

void move_cursor();
void scroll();

void print(const char* str);
void newline();
void clear();

int strcmp(const char* a, const char* b);
int starts_with(const char* str, const char* prefix);


/* =========================
   VGA TEXT MODE
========================= */

volatile char* video = (volatile char*)0xB8000;

int cursor = 0;


/* =========================
   KEYBOARD STATE
========================= */

int shift_pressed = 0;


/* =========================
   INPUT BUFFER
========================= */

char input[256];
int input_pos = 0;


/* =========================
   SCANCODE TABLES
========================= */

char scancode_table[128] = {
0,27,'1','2','3','4','5','6','7','8','9','0','-','=',8,9,
'Q','W','E','R','T','Y','U','I','O','P','[',']','\n',0,'A','S',
'D','F','G','H','J','K','L',';','\'','`',0,'\\','Z','X','C','V',
'B','N','M',',','.','/',0,'*',0,' '
};

char scancode_table_shift[128] = {
0,27,'!','@','#','$','%','^','&','*','(',')','_','+',8,9,
'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,'A','S',
'D','F','G','H','J','K','L',':','"','~',0,'|','Z','X','C','V',
'B','N','M','<','>','?',0,'*',0,' '
};


/* =========================
   PORT IO
========================= */

unsigned char inb(unsigned short port) {

    unsigned char result;

    __asm__ volatile ("inb %1, %0"
        : "=a"(result)
        : "Nd"(port));

    return result;
}

void outb(unsigned short port, unsigned char val) {

    __asm__ volatile ("outb %0, %1"
        :
        : "a"(val), "Nd"(port));
}


/* =========================
   HARDWARE CURSOR
========================= */

void move_cursor() {

    unsigned short pos = cursor / 2;

    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);

    outb(0x3D4, 15);
    outb(0x3D5, pos);
}


/* =========================
   SCREEN FUNCTIONS
========================= */

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

void print(const char* str) {

    while (*str) {

        video[cursor++] = *str++;
        video[cursor++] = 0x07;

        if (cursor >= 80 * 25 * 2) {
            scroll();
        }
    }

    move_cursor();
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


/* =========================
   STRING FUNCTIONS
========================= */

int strcmp(const char* a, const char* b) {

    while (*a && *b) {

        if (*a != *b) {
            return 0;
        }

        a++;
        b++;
    }

    return (*a == 0 && *b == 0);
}

int starts_with(const char* str, const char* prefix) {

    while (*prefix) {

        if (*str != *prefix) {
            return 0;
        }

        str++;
        prefix++;
    }

    return 1;
}


/* =========================
   BASIC COMMANDS
========================= */

void execute_basic() {

    newline();

    /* ===== PRINT ===== */

    if (starts_with(input, "PRINT ")) {

        print(input + 6);
    }

    /* ===== HELP ===== */

    else if (strcmp(input, "HELP")) {

        print("COMMANDS:");
        newline();

        print("PRINT TEXT");
        newline();

        print("CLEAR");
        newline();

        print("VER");
        newline();

        print("HELP");
    }

    /* ===== VERSION ===== */

    else if (strcmp(input, "VER")) {

        print("vnBASIC V0.1");
    }

    /* ===== CLEAR ===== */

    else if (strcmp(input, "CLEAR")) {

        clear();
    }

    /* ===== UNKNOWN ===== */

    else {

        print("?SYNTAX ERROR");
    }

    input_pos = 0;
    input[0] = 0;

    newline();
    print("] ");
}


/* =========================
   KEYBOARD DRIVER
========================= */

char get_key() {

    unsigned char scancode;

    while (1) {

        if (inb(0x64) & 1) {

            scancode = inb(0x60);

            /* LEFT SHIFT DOWN */
            if (scancode == 42) {
                shift_pressed = 1;
                continue;
            }

            /* LEFT SHIFT UP */
            if (scancode == 170) {
                shift_pressed = 0;
                continue;
            }

            /* RIGHT SHIFT DOWN */
            if (scancode == 54) {
                shift_pressed = 1;
                continue;
            }

            /* RIGHT SHIFT UP */
            if (scancode == 182) {
                shift_pressed = 0;
                continue;
            }

            /* ignore key releases */
            if (scancode & 0x80) {
                continue;
            }

            if (shift_pressed) {
                return scancode_table_shift[scancode];
            }

            return scancode_table[scancode];
        }
    }
}


/* =========================
   KERNEL ENTRY
========================= */

void kernel_main() {

    clear();

    print("vnBASIC V0.1");
    newline();

    print("READY.");
    newline();

    print("] ");

    while (1) {

        char c = get_key();

        /* ===== ENTER ===== */

        if (c == '\n') {

            input[input_pos] = 0;

            execute_basic();
        }

        /* ===== BACKSPACE ===== */

        else if (c == 8 && input_pos > 0) {

            input_pos--;

            cursor -= 2;

            video[cursor] = ' ';
            video[cursor + 1] = 0x07;

            move_cursor();
        }

        /* ===== NORMAL CHARACTER ===== */

        else if (c > 0) {

            if (input_pos < 255) {
                input[input_pos++] = c;
            }

            video[cursor++] = c;
            video[cursor++] = 0x07;

            move_cursor();

            if (cursor >= 80 * 25 * 2) {
                scroll();
            }
        }
    }
}