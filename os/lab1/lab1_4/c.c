#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

void cleanup(void* data) {
    printf("cleanup\n");
    free(data);
}

void* pthreadMain(void* arg) {
    char* message = (char*)malloc(sizeof(char) * 12);
    strncpy(message, "Hello World", 12);

    // регистрируем функцию
    pthread_cleanup_push(cleanup, message);

    while(1) {
        printf("%s\n", message);
        sleep(1);
    }
    // 1 - cleanup / 0 - no cleanup
    //    pthread_cleanup_pop(1);

    return NULL;
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
}
