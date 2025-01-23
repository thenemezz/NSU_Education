#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

void* pthread2Main(void* arg) {
    sigset_t oldset;

    if (pthread_sigmask(SIG_BLOCK, NULL, &oldset) != 0) {
        printf("Error getting signal mask: %s\n", strerror(errno));
        return NULL;
    }

    printf("Current signal mask:\n");
    for (int i = 1; i < NSIG; i++) {
        if (sigismember(&oldset, i)) {
            printf("Signal %d is blocked\n", i);
        } else {
            printf("Signal %d is not blocked\n", i);
        }
    }

    return NULL;
}

int main() {
    pthread_t thread;

    if (pthread_create(&thread, NULL, pthread2Main, NULL) != 0) {
        printf("Error creating thread: %s\n", strerror(errno));
        return 1;
    }

    pthread_join(thread, NULL);
    return 0;
}
