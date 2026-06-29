#ifndef FS_H
#define FS_H

#define MAX_FILES 32
#define NAME_LEN 128
#define CONTENT_LEN 256

extern char LOADED_CONTENT[256];

void save_file(char* name, char* content);
int load_file(char* name);
void list_files();

#endif