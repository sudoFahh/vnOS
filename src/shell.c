#include "shell.h"
#include "screen.h"
#include "util.h"
#include "fs.h"

char input[256];
int input_pos = 0;

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

        char name[128];
        char content[256];

        int i = 5;
        int n = 0;

        while (input[i] != ' ' && input[i] != 0) {

            name[n++] = input[i++];
        }

        name[n] = 0;

        if (input[i] == ' ') {
            i++;
        }

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

        char name[128];

        int i = 5;
        int n = 0;

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