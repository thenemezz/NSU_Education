#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* mythread(void* arg) {
    printf("Thread is running...\n");
    sleep(2);
    printf("Thread is done!\n");
    return NULL;
}

int main() {
    pthread_t thread;
    int err;

    err = pthread_create(&thread, NULL, mythread, NULL);
    if (err != 0) {
        printf("Error: Unable to create thread\n");
        return -1;
    }

    printf("Main thread is waiting for the other thread to finish...\n");
    pthread_join(thread, NULL);
    printf("Main thread: Other thread has finished!\n");

    return 0;
}
