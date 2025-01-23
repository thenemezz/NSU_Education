#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
    return NULL;
}

int main() {
    pthread_t tid;
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());
    printf("11111\n");
    err = pthread_create(&tid, NULL, mythread, NULL);
    printf("22222\n");
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    // Ожидание завершения дочернего потока
    pthread_join(tid, NULL);

    return 0;
}
