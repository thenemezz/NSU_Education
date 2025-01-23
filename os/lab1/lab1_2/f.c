#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_attr_t attr;

void sigint_handler(int signum) {
    printf("\nReceived SIGINT, cleaning up and exiting...\n");

    pthread_attr_destroy(&attr);

    _exit(0);
}

void* mythread(void* arg) {
    printf("Thread with TID %lu is running and detached...\n", pthread_self());

    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    int err;

    signal(SIGINT, sigint_handler);

    pthread_attr_init(&attr);

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    while (1) {
        err = pthread_create(&thread, &attr, mythread, NULL);
        if (err != 0) {
            printf("Error: Unable to create thread\n");
            return -1;
        }

    }

    return 0;
}
