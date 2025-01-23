#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int global_var = 0;

void *mythread(void *arg) {
    int local_var = 0;

    int big_array[100000];

    printf("Thread [%d] sleeping for 15 seconds before work\n", gettid());
    sleep(15);

    // Инициализация массива
    for (int i = 0; i < 100000; i++) {
        big_array[i] = i;
    }

    for (int i = 0; i < 100000; i++) {
        local_var += big_array[i];
    }

    global_var++;

    printf("Thread [%d] started\n", gettid());
    printf("  Global variable (after increment): %d (address: %p)\n", global_var, (void*)&global_var);
    printf("  Local variable (after first increment): %d (address: %p)\n", local_var, (void*)&local_var);
    printf("  Big array address: %p\n", (void*)big_array);

    //  цикл, работающий с локальной переменной
    for (int i = 0; i < 100000; i++) {
        local_var -= big_array[i];
    }

    printf("Thread [%d] finished\n", gettid());
    printf("  Local variable (after second decrement): %d (address: %p)\n", local_var, (void*)&local_var);

    sleep(15);

    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    int err;

    err = pthread_create(&tid1, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed for thread 1\n");
        return -1;
    }

    err = pthread_create(&tid2, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed for thread 2\n");
        return -1;
    }


    // PID процесса для изучения /proc/[pid]/maps
    printf("Main thread PID: %d\n", getpid());

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}
