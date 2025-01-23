#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* mythread(void* arg) {
    printf("Thread is running...\n");
    sleep(2);
    int* result = (int*)malloc(sizeof(int));
    *result = 42;
    printf("Thread is done and returning 42!\n");
    pthread_exit((void*)result);
}

int main() {
    pthread_t thread;
    int* thread_result;
    int err;

    err = pthread_create(&thread, NULL, mythread, NULL);
    if (err != 0) {
        printf("Error: Unable to create thread\n");
        return -1;
    }

    printf("Main thread is waiting for the other thread to finish...\n");
    pthread_join(thread, (void**)&thread_result);

    printf("Main thread: Other thread returned %d!\n", *thread_result);

    free(thread_result);

    return 0;
}
