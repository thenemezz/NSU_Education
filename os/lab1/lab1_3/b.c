#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct thread_data {
    int id;
    char* message;
};

void* mythread(void* arg) {
    struct thread_data* data = (struct thread_data*)arg;

    printf("Thread ID: %d\n", data->id);
    printf("Thread message: %s\n", data->message);

    sleep(2);

    free(data->message);
    free(data);

    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    int err;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    struct thread_data* data = (struct thread_data*)malloc(sizeof(struct thread_data));
    if (data == NULL) {
        printf("Error allocating memory for thread_data\n");
        return -1;
    }

    data->id = 1;
    data->message = (char*)malloc(50 * sizeof(char));

    strcpy(data->message, "Hello from the detached thread!");

    err = pthread_create(&thread, &attr, mythread, (void*)data);
    if (err != 0) {
        printf("Error: Unable to create thread\n");
        free(data->message);
        free(data);
        return -1;
    }

    pthread_attr_destroy(&attr);

    printf("Main thread finished, detached thread is still running.\n");

    pthread_exit(NULL);
    return 0;
}
