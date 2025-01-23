#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void *mythread(void *arg) {
    pthread_t thread_id = pthread_self();
    pthread_t *created_thread_id = (pthread_t *)arg;

    printf("ID PROCESS ==  mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());

    printf("Thread ID (pthread_self): %lu\n", thread_id);

    printf("Thread ID (from pthread_create): %lu\n", *created_thread_id);

    if (pthread_equal(thread_id, *created_thread_id)) {
        printf("Thread IDs are equal!\n");
    } else {
        printf("Thread IDs are different!\n");
    }

    return NULL;
}

int main() {
    pthread_t tid;
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    pthread_t *tid_arg = malloc(sizeof(pthread_t));
    if (tid_arg == NULL) {
        perror("Failed to allocate memory");
        return -1;
    }

    // Создание потока
    err = pthread_create(&tid, NULL, mythread, tid_arg);
    if (err) {
        printf("main: pthread_create() failed\n");
        free(tid_arg);
        return -1;
    }

    *tid_arg = tid;

    pthread_join(tid, NULL);

    free(tid_arg);

    return 0;
}
