#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "fileutils.h"

#define MAX_THREAD_COUNT 8

int main(int argc, char* argv[]) {
    const char* baseDirectory = argv[1];
    const char* filename = argv[2];
    int numDirectories;
    char** directories = listDirectories(baseDirectory, &numDirectories);

    if (!directories) {
        printf("Error listing directories.\n");
        return 1;
    }

    pthread_t threads[MAX_THREAD_COUNT];
    int thread_count = 0;

    // Create a pool of threads to search for the file
    for (int i = 0; i < numDirectories; i++) {
        // Check if the file is already found, no need to create more threads
        if (!file_found && (thread_count < MAX_THREAD_COUNT)) {
            SearchArgs* searchArgs = malloc(sizeof(SearchArgs));
            searchArgs->directory = malloc(strlen(directories[i]) + 1);
            strcpy(searchArgs->directory, directories[i]);
            searchArgs->filename = malloc(strlen(filename) + 1);
            strcpy(searchArgs->filename, filename);

            pthread_create(&threads[thread_count], NULL, searchFile, (void*)searchArgs);
            printf("Thread [%lu] created to search in: %s\n", threads[thread_count], searchArgs->directory);
            thread_count++;
        } else {
            // Wait for threads to finish if the maximum thread count is reached
            for (int i = 0; i < thread_count; i++) {
                pthread_join(threads[i], NULL);
                printf("Thread [%lu] finnished.\n", threads[i]);
            }
            thread_count = 0;
        }
    }

    // Free allocated memory for directories
    for (int i = 0; i < numDirectories; i++) {
        free(directories[i]);
    }
    free(directories);

    printf("Program finnished\n");

    return 0;
}