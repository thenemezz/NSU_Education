#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

struct thread_data {
    int id;
    char* message;
};

void* mythread(void* arg) {
    struct thread_data* data = (struct thread_data*)arg;

    printf("Thread ID: %d\n", data->id);
    printf("Thread message: %s\n", data->message);

    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    int err;

    struct thread_data data;
    data.id = 1;
    data.message = "Hello from the thread!";

    err = pthread_create(&thread, NULL, mythread, (void*)&data);
    if (err != 0) {
        printf("Error: Unable to create thread\n");
        return -1;
    }

    pthread_join(thread, NULL);

    return 0;
}
