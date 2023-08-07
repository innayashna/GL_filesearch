#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>

#include "fileutils.h"

#define FULL_PATH_MAX_LENGTH 2048

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int file_found = 0;

// Function to get a list of directories in the base directory
char** listDirectories(const char* baseDirectory, int* numDirectories) {
    DIR* dir = opendir(baseDirectory);
    if (!dir) {
        *numDirectories = 0;
        return NULL;
    }

    char** directories = NULL;
    int count = 0;
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // Skip the "." and ".." directories
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            char fullpath[FULL_PATH_MAX_LENGTH];
            snprintf(fullpath, sizeof(fullpath), "%s%s", baseDirectory, entry->d_name);

            char* directoryPath = malloc(strlen(fullpath) + 1); // Allocate memory for the full path
            strcpy(directoryPath, fullpath);

            directories = realloc(directories, (count + 1) * sizeof(char*));
            directories[count] = directoryPath;
            count++;
        }
    }

    closedir(dir);
    *numDirectories = count;
    return directories;
}


// Function to search for the file in a given directory
void* searchFile(void* args) {
    pthread_mutex_lock(&mutex);
    if (file_found) {
        printf("Thread [%lu] finnished searching since file was found by other thread\n", pthread_self());
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
        return (void*)NULL;
    }
    pthread_mutex_unlock(&mutex);

    SearchArgs* searchArgs = (SearchArgs*)args;
    const char* directory = searchArgs->directory;
    const char* filename = searchArgs->filename;

    DIR* dir = opendir(directory);
    if (!dir) {
        return (void*)NULL;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[FULL_PATH_MAX_LENGTH];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", directory, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) == -1) {
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            SearchArgs newArgs;
            newArgs.directory = fullpath;
            newArgs.filename = filename;
            searchFile(&newArgs);
        } else if (strcmp(entry->d_name, filename) == 0) {
            printf("Thread [%lu] Found file: %s/%s\n", pthread_self(), directory, entry->d_name);
            closedir(dir);
            pthread_mutex_lock(&mutex);
            file_found = 1;
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
            break;
        }
    }

    closedir(dir);
    return (void*)NULL;
}