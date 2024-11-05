#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

#include "mythread/mythread.h"

mythread_t threads[10];
int indexes[10];

void* thread_func(void* arg) {
    char* result = (char*) malloc(20);
    sprintf(result, "Hello from %d thread", *(int*) arg);
    return result;
}

int main(int argc, char** argv) {

    for (int i = 0; i < 10; ++i) {
        indexes[i] = i;
        mythread_create(&threads[i], thread_func, &indexes[i]);
    }

    for (int i = 0; i < 10; ++i) {
        char* result;
        mythread_join(threads[i], (void**) &result);
        sleep(2);

        printf("%lu: %s\n", threads[i].thread_id, result);
        free(result);
    }

    mythread_t thread;
    int arg = 9;
    char* result;

    mythread_create(&thread, thread_func, (void*) &arg);

    mythread_join(thread, (void**) &result);
    printf("%lu: %s\n", thread.thread_id, result);

    return 0;
}

