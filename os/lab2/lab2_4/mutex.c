#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sched.h>
#include <sys/syscall.h>
#include <linux/futex.h>

enum {
    STOPPING_ORDER = 100,
    COUNT_ITERATION = 10000000,
    STATUS_LOCK = 0,
    STATUS_UNLOCK = 1,
};

typedef struct {
    int lock;
} mutex_t;

int counter = 0;
//  atomic_compare_exchange_strong,
//  https://elixir.bootlin.com/glibc/glibc-2.37/source/sysdeps/nptl/futex-internal.h#L317
void set_cpu(int n) {
	int err;
	cpu_set_t cpuset;
	pthread_t tid = pthread_self();

	CPU_ZERO(&cpuset);
	CPU_SET(n, &cpuset);

	err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
	if (err) {
		printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
		return;
	}

	printf("set_cpu: set cpu %d\n", n);
}

void mutex_init(mutex_t* m){
    m->lock = 1;
}

static int wrapper_futex(int* uaddr, int futex_op, int val, const struct timespec* timeout,
                         int* uaddr2, int val3) {
    return syscall(SYS_futex, uaddr, futex_op,
                   val, timeout, uaddr, val3);
}

void mutex_lock(mutex_t* m) {
    while (true) {
        int expected = STATUS_UNLOCK;
        if (atomic_compare_exchange_strong(&m->lock, &expected, STATUS_LOCK))
            break;
        int err = wrapper_futex(&m->lock, FUTEX_WAIT, STATUS_LOCK, NULL, NULL, 0);
        if (err == -1 && errno != EAGAIN)
            fprintf(stderr, "futex FUTEX_WAIT failed %s\n", strerror(errno));
        sched_yield();
    }
}

void mutex_unlock(mutex_t *m) {
    int expected = STATUS_LOCK;
    if(atomic_compare_exchange_strong(&m->lock, &expected, STATUS_UNLOCK)) {
        int err = wrapper_futex(&m->lock, FUTEX_WAKE, STATUS_UNLOCK, NULL, NULL, 0);
        if(err == -1 && errno != EAGAIN) fprintf(stderr, "futex FUTEX_WAKE failed %s\n", strerror(errno));
    }
}

int join_thread(pthread_t tid) {
    void* ret_val;
    int err = pthread_join(tid, &ret_val);
    if (err) fprintf(stderr, "queue_destroy: pthread_join() failed %s\n", strerror(err));
    return err;
}

void* thread_1(void* args) {
    set_cpu(2);
    mutex_t* m = (mutex_t*) args;
    assert(m != NULL);

    for(int i = 0; i < COUNT_ITERATION; ++i) {
        mutex_lock(m);
        ++counter;
//        printf("thread_1: count iteration %d\n", i);
        mutex_unlock(m);
    }

    return NULL;
}

void* thread_2(void* args) {
    set_cpu(1);
    mutex_t* m = (mutex_t*) args;
    assert(m != NULL);

    for(int i = 0; i < COUNT_ITERATION; ++i) {
        mutex_lock(m);
        ++counter;
//        printf("thread_2: count iteration %d\n", i);
        mutex_unlock(m);
    }
    return NULL;
}

int main() {
    mutex_t m;

    mutex_init(&m);

    pthread_t tid_1, tid_2;
    int err = pthread_create(&tid_1, NULL, thread_1, &m);
	if (err) {
        printf("main: pthread_create(): thread_1 failed: %s\n", strerror(err));
        return EXIT_FAILURE;
    }

    err = pthread_create(&tid_2, NULL, thread_2, &m);
	if (err) {
		printf("main: pthread_create(): thread_2 failed: %s\n", strerror(err));
        join_thread(tid_1);
		return EXIT_FAILURE;
	}

    int err_2 = join_thread(tid_1);
    int err_1 = join_thread(tid_2);

    printf("counter %d\n", counter);
    return err_1 || err_2 ? EXIT_FAILURE : EXIT_SUCCESS;
}