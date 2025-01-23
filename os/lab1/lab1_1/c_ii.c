#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

// Глобальная переменная
int global_var = 0;
// Локальная статическая переменная
static int static_var = 0;

void *mythread(void *arg) {
    // Локальная переменная
    int local_var = 0;
    // Локальная константная переменная
    const int const_local_var = 1;

    printf("Addresses in mythread [%d]:\n", gettid());
    printf("  Global variable: %p\n", (void*)&global_var);
    printf("  Static variable: %p\n", (void*)&static_var);
    printf("  Local variable: %p\n", (void*)&local_var);
    printf("  Const local variable: %p\n", (void*)&const_local_var);

    return NULL;
}

int main() {
    pthread_t tid;
    int err;

    // Локальная переменная
    int local_var_main = 0;
    // Локальная константная
    const int const_local_var_main = 1;

    // Вывод адресов переменных в main
    printf("Addresses in main [%d]:\n", gettid());
    printf("  Global variable: %p\n", (void*)&global_var);
    printf("  Static variable: %p\n", (void*)&static_var);
    printf("  Local variable: %p\n", (void*)&local_var_main);
    printf("  Const local variable: %p\n", (void*)&const_local_var_main);

    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed\n");
        return -1;
    }

    pthread_join(tid, NULL);

    return 0;
}
