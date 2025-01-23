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

enum {
    STOPPING_ORDER = 100,
    COUNT_ITERATION = 10000000,
    STATUS_LOCK = 0,
    STATUS_UNLOCK = 1,
};

typedef struct {
    atomic_int lock;
} spinlock_t;

int counter = 0;

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

void spinlock_init(spinlock_t* s){
    s->lock = STATUS_UNLOCK;
}

void spinlock_lock(spinlock_t* s) {
        while(true) {
         int expected = STATUS_UNLOCK;
         if(atomic_compare_exchange_strong(&s->lock, &expected, STATUS_LOCK))
             break;
    }
}

void spinlock_unlock(spinlock_t *s) {
    int expected = STATUS_LOCK;
    atomic_compare_exchange_strong(&s->lock, &expected, STATUS_UNLOCK);
}

int join_thread(pthread_t tid) {
    void* ret_val;
    int err = pthread_join(tid, &ret_val);
    if (err)
        fprintf(stderr, "queue_destroy: pthread_join() failed %s\n", strerror(err));
    return err;
}

void* thread_1(void* args) {
    set_cpu(2);
    spinlock_t* s = (spinlock_t*) args;
    assert(s != NULL);

    for(int i = 0; i < COUNT_ITERATION; ++i) {
        spinlock_lock(s);
        ++counter;
        //printf("thread_1: count iteration %d\n", i);
        spinlock_unlock(s);
//        usleep(random() % STOPPING_ORDER);
    }

    return NULL;
}

void* thread_2(void* args) {
    set_cpu(1);
    spinlock_t* s = (spinlock_t*) args;
    assert(s != NULL);

    for(int i = 0; i < COUNT_ITERATION; ++i) {
        spinlock_lock(s);
        ++counter;
//        printf("thread_2: count iteration %d\n", i);
        spinlock_unlock(s);
//        usleep(random() % STOPPING_ORDER);
    }
    return NULL;
}

int main() {
    spinlock_t s;

    spinlock_init(&s);

    pthread_t tid_1, tid_2;
    int err = pthread_create(&tid_1, NULL, thread_1, &s);
	if (err) {
        printf("main: pthread_create(): thread_1 failed: %s\n", strerror(err));
        return EXIT_FAILURE;
    }

    err = pthread_create(&tid_2, NULL, thread_2, &s);
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