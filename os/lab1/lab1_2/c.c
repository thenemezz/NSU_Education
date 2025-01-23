#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void* mythread(void* arg) {
    printf("Thread is running...\n");
    sleep(2);

    char* result = (char*)malloc(50 * sizeof(char));
    if (result == NULL) {
        printf("Error allocating memory\n");
        pthread_exit(NULL);
    }

    strcpy(result, "hello world");

    printf("Thread is done and returning string: %s\n", result);
    pthread_exit((void*)result);
}

int main() {
    pthread_t thread;
    char* thread_result;
    int err;

    err = pthread_create(&thread, NULL, mythread, NULL);
    if (err != 0) {
        printf("Error: Unable to create thread\n");
        return -1;
    }

    printf("Main thread is waiting for the other thread to finish...\n");
    pthread_join(thread, (void**)&thread_result);

    if (thread_result != NULL) {
        printf("Main thread: Other thread returned string: %s\n", thread_result);

        free(thread_result);
    } else {
        printf("Main thread: Thread returned NULL\n");
    }

    return 0;
}
