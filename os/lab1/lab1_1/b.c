#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    int thread_num = *(int *)arg;
    printf("mythread [%d %d %d]: Hello from mythread! Thread number: %d\n", getpid(), getppid(), gettid(), thread_num);
    return NULL;
}

int main() {
    pthread_t tids[5];
    int err;
    int args[5];

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    for (int i = 0; i < 5; i++) {
        args[i] = i;
        err = pthread_create(&tids[i], NULL, mythread, &args[i]);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(tids[i], NULL);
    }

    return 0;
}
