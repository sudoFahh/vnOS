#include "shell.h"
#include "screen.h"
#include "util.h"
#include "fs.h"

char input[256];
int input_pos = 0;

int edit_mode = 0;

char edit_filename[128];
char edit_buffer[2048];
int edit_buffer_len = 0;

void handle_edit_line() {

    if (strcmp(input, ":WQ")) {

        save_file(edit_filename, edit_buffer);
        edit_mode = 0;

        newline();
        print("SAVED");
        newline();
        print("] ");
    }

    else if (strcmp(input, ":W")) {

        save_file(edit_filename, edit_buffer);

        newline();
        print("SAVED");
        newline();
        print("> ");
    }

    else if (strcmp(input, ":Q")) {

        edit_mode = 0;
        edit_buffer_len = 0;
        edit_buffer[0] = 0;

        newline();
        print("] ");
    }

    else {

        int i = 0;

        while (input[i] != 0 && edit_buffer_len < 2046) {
            edit_buffer[edit_buffer_len++] = input[i++];
        }

        edit_buffer[edit_buffer_len++] = '\n';
        edit_buffer[edit_buffer_len] = 0;

        newline();
        print("> ");
    }

    input_pos = 0;
    input[0] = 0;
}

void execute_command() {

    newline();

    /* =====================================
       PRINT
    ===================================== */

    if (starts_with(input, "PRINT ") && !edit_mode) {

        print(input + 6);
    }

    /* =====================================
       HELP
    ===================================== */

    else if (strcmp(input, "HELP") && !edit_mode) {

        print("COMMANDS:");
        newline();

        print("PRINT TEXT");
        newline();

        print("MATH X OP Y");
        newline();

        print("LS");
        newline();

        print("EDIT FILENAME CONTENT");
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

    else if (strcmp(input, "VER") && !edit_mode) {

        print("vnOS V0.1");
    }

    /* =====================================
       CLEAR
    ===================================== */

    else if (strcmp(input, "CLEAR") && !edit_mode) {

        clear();
    }

    /* =====================================
       LIST FILES
    ===================================== */

    else if (strcmp(input, "LS") && !edit_mode) {

        list_files();
    }

    /* =====================================
       RUN FILES
    ===================================== */

    else if (strcmp(input, "RUN") && !edit_mode) {

        strcpy(input, LOADED_CONTENT);
        execute_command();
    }

    /* =====================================
       SAVING FILES
    ===================================== */

    else if (starts_with(input, "EDIT ")) {

        edit_mode = 1;

        int i = 5;
        int n = 0;

        while (input[i] != ' ' && input[i] != 0) {
            edit_filename[n++] = input[i++];
        }

        edit_filename[n] = 0;

        clear();
        print("EDIT MODE: ");
        print(edit_filename);
        newline();

        if (load_file(edit_filename)) {

            strcpy(edit_buffer, LOADED_CONTENT);

            edit_buffer_len = 0;
            while (edit_buffer[edit_buffer_len] != 0) {
                edit_buffer_len++;
            }

            int j = 0;
            while (LOADED_CONTENT[j] != 0) {

                if (LOADED_CONTENT[j] == '\n') {
                    newline();
                } else {
                    put_char(LOADED_CONTENT[j]);
                }
                j++;
            }

            newline();
        }
        else {

            edit_buffer_len = 0;
            edit_buffer[0] = 0;
        }

        print("> ");

        input_pos = 0;
        input[0] = 0;

        return;
    }   

    /* =====================================
       LOADING FILES
    ===================================== */

    else if (starts_with(input, "LOAD ") && !edit_mode) {

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

    else if (starts_with(input, "MATH ") && !edit_mode) {

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
        if(!edit_mode){
        print("?SYNTAX ERROR");
        }
    }

done:

    input_pos = 0;
    input[0] = 0;

    newline();
    if (!edit_mode) {
        print("] ");
    }
}