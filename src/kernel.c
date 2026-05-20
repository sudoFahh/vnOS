#include "screen.h"
#include "keyboard.h"
#include "shell.h"

void kernel_main() {

    clear();

    print("vnBASIC V0.1");
    newline();

    print("READY.");
    newline();

    print("] ");

    while (1) {

        char c = get_key();

        if (c == '\n') {

            input[input_pos] = 0;
            execute_command();
        }

        else if (c == 8 && input_pos > 0) {

            input_pos--;
            backspace();
        }

        else if (c > 0) {

            if (input_pos < 255) {
                input[input_pos++] = c;
            }

            put_char(c);
        }
    }
}
