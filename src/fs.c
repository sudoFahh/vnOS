#include "fs.h"
#include "screen.h"
#include "util.h"

char names[MAX_FILES][NAME_LEN];
char contents[MAX_FILES][CONTENT_LEN];
int used[MAX_FILES];

char LOADED_CONTENT[256];

void save_file(char* name, char* content) {

    for (int i = 0; i < MAX_FILES; i++) {

        if (!used[i] || strcmp(names[i], name)) {

            int j = 0;

            while (name[j] && j < NAME_LEN - 1) {
                names[i][j] = name[j];
                j++;
            }

            names[i][j] = 0;

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