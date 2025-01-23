#pragma once

#include <pthread.h>
#include <stdbool.h>

#define MAX_THREADS 20
#define EXECUTOR_QUEUE_SIZE 100

typedef struct ExecutorTask {
    void (*function)(void*);
    void *argument;
    struct ExecutorTask *next;
} ExecutorTask;

typedef struct {
    ExecutorTask *head;
    ExecutorTask *tail;
    size_t count;
} TaskQueue;

typedef struct {
    pthread_t *threads;
    size_t thread_count;

    TaskQueue queue;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;

    volatile bool running;
    volatile size_t active_tasks;
} ThreadExecutor;

ThreadExecutor* executor_create(void);

bool executor_submit(ThreadExecutor *exec, void (*func)(void*), void *arg);

void executor_shutdown(ThreadExecutor *exec);