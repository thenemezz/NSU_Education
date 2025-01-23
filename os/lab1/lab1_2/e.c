#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* mythread(void* arg) {
    pthread_detach(pthread_self());
    printf("Thread with TID %lu is running and detached...\n", pthread_self());

    //    sleep(2);

    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    int err;


    err = pthread_create(&thread, NULL, mythread, NULL);
    if (err != 0) {
        printf("Error: Unable to create thread\n");
        return -1;
    }
    sleep(5);
    printf("JOIN");
    if (pthread_join(thread, NULL) != 0){
        printf("WARNING");

    }
    sleep(7);

    return 0;
}
