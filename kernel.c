#include <stddef.h>

/* =========================
   FUNCTION DECLARATIONS
========================= */

void execute_command();
char get_key();

int atoi(const char* str);
void print_int(int num);

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

#define MAX_FILES 32
#define NAME_LEN 128
#define CONTENT_LEN 256

char names[MAX_FILES][NAME_LEN];
char contents[MAX_FILES][CONTENT_LEN];
int used[MAX_FILES];

char LOADED_CONTENT[256];


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

void strcpy(char* dest, const char* src) {

    int i = 0;

    while (src[i] != 0) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = 0;
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
    INTEGER FUNCTIONS
========================= */

int atoi(const char* str) {

    int result = 0;
    int sign = 1;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    while (*str >= '0' && *str <= '9') {

        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}

/* =========================
    FILESYSTEM COMMANDS
========================= */

void save_file(char* name, char* content) {

    for (int i = 0; i < MAX_FILES; i++) {

        // empty slot OR overwrite same file
        if (!used[i] || strcmp(names[i], name)) {

            int j = 0;

            // copy name
            while (name[j] && j < NAME_LEN - 1) {
                names[i][j] = name[j];
                j++;
            }
            names[i][j] = 0;

            // copy content
            j = 0;
            while (content[j] && j < CONTENT_LEN - 1) {
                contents[i][j] = content[j];
                j++;
            }
            contents[i][j] = 0;

            used[i] = 1;

            print("SAVED");
            return;
        }
    }

    print("DISK FULL");
}

void load_file(char* name) {

    for (int i = 0; i < MAX_FILES; i++) {

        if (used[i] && strcmp(names[i], name)) {

            strcpy(LOADED_CONTENT, contents[i]);
            print("LOADED");
            return;
        }
    }

    print("NOT FOUND");
}

void list_files() {

    for (int i = 0; i < MAX_FILES; i++) {

        if (used[i]) {
            print(names[i]);
            newline();
        }
    }
}

/* =========================
   BASIC COMMANDS
========================= */

void execute_command() {

    newline();

    /* =====================================
       PRINT
    ===================================== */

    if (starts_with(input, "PRINT ")) {

        print(input + 6);
    }

    /* =====================================
       HELP
    ===================================== */

    else if (strcmp(input, "HELP")) {

        print("COMMANDS:");
        newline();

        print("PRINT TEXT");
        newline();

        print("MATH X OP Y");
        newline();

        print("LS");
        newline();

        print("SAVE FILENAME CONTENT");
        newline();

        print("LOAD FILENAME");
        newline();

        print("RUN");
        newline();

        print("CLEAR");
        newline();

        print("VER");
        newline();

        print("HELP");
    }

    /* =====================================
       VERSION
    ===================================== */

    else if (strcmp(input, "VER")) {

        print("vnOS V0.1");
    }

    /* =====================================
       CLEAR
    ===================================== */

    else if (strcmp(input, "CLEAR")) {

        clear();
    }

    /* =====================================
       LIST FILES
    ===================================== */


    else if (strcmp(input, "LS")) {
    list_files();
    }

    /* =====================================
       RUN FILES
    ===================================== */


    else if (strcmp(input, "RUN")) {

    strcpy(input, LOADED_CONTENT);
    execute_command();
    }

    /* =====================================
       SAVING FILES
    ===================================== */

    else if (starts_with(input, "SAVE ")) {

    char name[NAME_LEN];
    char content[CONTENT_LEN];

    int i = 5, n = 0;

    // read name
    while (input[i] != ' ' && input[i] != 0) {
        name[n++] = input[i++];
    }
    name[n] = 0;

    if (input[i] == ' ') i++;

    // read content
    int c = 0;
    while (input[i] != 0) {
        content[c++] = input[i++];
    }
    content[c] = 0;

    save_file(name, content);
    }

    /* =====================================
       LOADING FILES
    ===================================== */

    else if (starts_with(input, "LOAD ")) {

    char name[NAME_LEN];

    int i = 5, n = 0;

    while (input[i] != 0) {
        name[n++] = input[i++];
    }
    name[n] = 0;

    load_file(name);
    }

    /* =====================================
       MATH
    ===================================== */

    else if (starts_with(input, "MATH ")) {

        int i = 5;

        char num1_str[16];
        char num2_str[16];

        int n1 = 0;
        int n2 = 0;

        char op;

        /* ===== FIRST NUMBER ===== */

        while (input[i] != ' ' && input[i] != 0) {

            num1_str[n1++] = input[i++];
        }

        num1_str[n1] = 0;

        i++;

        /* ===== OPERATOR ===== */

        op = input[i++];

        i++;

        /* ===== SECOND NUMBER ===== */

        while (input[i] != 0) {

            num2_str[n2++] = input[i++];
        }

        num2_str[n2] = 0;

        int a = atoi(num1_str);
        int b = atoi(num2_str);

        int result = 0;

        /* ===== OPERATIONS ===== */

        if (op == '+') {

            result = a + b;
        }

        else if (op == '-') {

            result = a - b;
        }

        else if (op == '*') {

            result = a * b;
        }

        else if (op == '/') {

            if (b == 0) {

                print("DIV BY ZERO");

                goto done;
            }

            result = a / b;
        }

        else {

            print("BAD OPERATOR");

            goto done;
        }

        print_int(result);
    }

    /* =====================================
       UNKNOWN
    ===================================== */

    else {

        print("?SYNTAX ERROR");
    }

done:

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

            execute_command();
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