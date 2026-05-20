#include "keyboard.h"
#include "io.h"

int shift_pressed = 0;

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

char get_key() {

    unsigned char scancode;

    while (1) {

        if (inb(0x64) & 1) {

            scancode = inb(0x60);

            if (scancode == 42 || scancode == 54) {
                shift_pressed = 1;
                continue;
            }

            if (scancode == 170 || scancode == 182) {
                shift_pressed = 0;
                continue;
            }

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
