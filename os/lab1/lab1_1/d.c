#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

int global_var = 0;
static int static_var = 0;

void *mythread(void *arg) {
    int local_var = 0;
    const int const_local_var = 1;

    // изменяем переменные
    global_var++;
    local_var++;

    printf("Thread [%d]:\n", gettid());
    printf("  Global variable (after increment): %d (address: %p)\n", global_var, (void*)&global_var);
    printf("  Static variable: %d (address: %p)\n", static_var, (void*)&static_var);
    printf("  Local variable (after increment): %d (address: %p)\n", local_var, (void*)&local_var);
    printf("  Const local variable: %d (address: %p)\n", const_local_var, (void*)&const_local_var);

    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    int err;

    err = pthread_create(&tid1, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed\n");
        return -1;
    }

    err = pthread_create(&tid2, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed\n");
        return -1;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("Main thread [%d]:\n", gettid());
    printf("  Global variable (final value): %d (address: %p)\n", global_var, (void*)&global_var);

    return 0;
}
