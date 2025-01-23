#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void* pthreadMain(void* arg) {
    //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    unsigned long long counter = 0;
    while(1) {
        //        pthread_testcancel();    //искуcтвенная точка отмены.
        counter++;
    }
}

int main(int argc, char** argv) {
    pthread_t thread;
    int error;

    error = pthread_create(&thread, NULL, pthreadMain, NULL);
    if (error) {
        printf("%s\n", strerror(error));
        return 1;
    }
    sleep(2);
    error = pthread_cancel(thread);
    if (error) {
        printf("%s\n", strerror(error));
        return 1;
    }

    error = pthread_join(thread, NULL);
    if (error) {
        printf("%s\n", strerror(error));
        return 1;
    }

    printf("Terminated\n");

    return 0;
}
