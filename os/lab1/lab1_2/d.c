#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void* mythread(void* arg) {
    printf("Thread with TID %lu is running...\n", pthread_self());
    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    int err;

    while (1) {
        err = pthread_create(&thread, NULL, mythread, NULL);
        if (err != 0) {
            printf("Error: Unable to create thread\n");
            return -1;
        }

        pthread_join(thread, NULL);
    }

    return 0;
}
