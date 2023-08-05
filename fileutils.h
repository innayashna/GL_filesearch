#ifndef FILEUTILS_H
#define FILEUTILS_H

extern int file_found;

typedef struct SearchArgs {
    char* directory;
    const char* filename;
} SearchArgs;

char** listDirectories(const char* baseDirectory, int* numDirectories);
void* searchFile(void* args);

#endif