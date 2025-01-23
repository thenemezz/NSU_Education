#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void handler(int signal) {
    char str[20];
    write(1, "thread: ", 8);
    sprintf(str, "%ld", pthread_self());
    write(1, str, strlen(str));
    write(1, " catched SIGINT\n", 16);
    // exit(0);
}

void* pthread2Main(void* arg) {
    sigset_t sigset;
    // sigemptyset - очищает набор сигналов + добавляем SIGINT
    if (sigemptyset(&sigset) || sigaddset(&sigset, SIGINT)) {
        printf("%s\n", strerror(errno));
        errno = 0;
        return NULL;
    }

    int error;

    if (error = pthread_sigmask(SIG_UNBLOCK, &sigset, NULL)) {
        printf("%s\n", strerror(error));
        return NULL;
    }

    if (signal(SIGINT, handler) == SIG_ERR) {
        printf("%s\n", strerror(errno));
        return NULL;
    }

    while (1) {

    }
}

void* pthread3Main(void* arg) {
    sigset_t sigset;
    if (sigemptyset(&sigset) || sigaddset(&sigset, SIGQUIT)) {
        printf("%s\n", strerror(errno));
        return NULL;
    }

    int error;

    if (error = pthread_sigmask(SIG_UNBLOCK, &sigset, NULL)) {
        printf("%s\n", strerror(error));
        return NULL;
    }

    while (1) {
        int sig;
        if (error = sigwait(&sigset, &sig)) {
            printf("%s\n", strerror(error));
            return NULL;
        }

        if (sig == SIGQUIT) {
            printf("SIGQUIT thread: %ld\n", pthread_self());
            exit(0);
        }
    }
}

int main(int argc, char** argv) {
    pthread_t thread1, thread2, thread3;
    int error;

    sigset_t mask;
    if (sigfillset(&mask) == -1) {
        printf("%s\n", strerror(errno));
        return 1;
    }
    if (error = pthread_sigmask(SIG_BLOCK, &mask, NULL)) {
        printf("%s\n", strerror(error));
        return 1;
    }

    printf("main thread(block everything): %ld\n", pthread_self());

    if (error = pthread_create(&thread2, NULL, pthread2Main, NULL)) {
        printf("%s\n", strerror(error));
        return 1;
    }
    printf("inter thread: %ld\n", thread2);

    if (error = pthread_create(&thread3, NULL, pthread3Main, NULL)) {
        printf("%s\n", strerror(error));
        return 1;
    }
    printf("waiter thread: %ld\n", thread3);

    while (1) {
        sleep(1);
    }

    return 0;
}
