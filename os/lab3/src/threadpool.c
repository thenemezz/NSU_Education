#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../header/cache.h"
#include "../header/threadpool.h"

#define COLOR_RED "\033[31m"
#define COLOR_BLUE "\033[34m"
#define RESET "\033[0m"


static void queue_init(TaskQueue *queue) {
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
}

static bool queue_push(TaskQueue *queue, void (*func)(void*), void *arg) {
    ExecutorTask *task = malloc(sizeof(ExecutorTask));
    if (!task) return false;

    task->function = func;
    task->argument = arg;
    task->next = NULL;

    if (queue->tail) {
        queue->tail->next = task;
        queue->tail = task;
    } else {
        queue->head = queue->tail = task;
    }

    queue->count++;
    return true;
}

static ExecutorTask* queue_pop(TaskQueue *queue) {
    if (!queue->head) return NULL;

    ExecutorTask *task = queue->head;
    queue->head = task->next;
    if (!queue->head) queue->tail = NULL;
    queue->count--;

    return task;
}

static void* executor_worker(void *arg) {
    ThreadExecutor *exec = (ThreadExecutor*)arg;

    while (true) {
        pthread_mutex_lock(&exec->mutex);

        while (exec->queue.count == 0 && exec->running) {
            pthread_cond_wait(&exec->not_empty, &exec->mutex);
        }

        if (!exec->running && exec->queue.count == 0) {
            pthread_mutex_unlock(&exec->mutex);
            break;
        }

        ExecutorTask *task = queue_pop(&exec->queue);
        exec->active_tasks++;

        pthread_cond_signal(&exec->not_full);
        pthread_mutex_unlock(&exec->mutex);

        if (task) {
            task->function(task->argument);
            free(task);

            pthread_mutex_lock(&exec->mutex);
            exec->active_tasks--;
            pthread_mutex_unlock(&exec->mutex);
        }
    }

    return NULL;
}

ThreadExecutor* executor_create(void) {
    ThreadExecutor *exec = malloc(sizeof(ThreadExecutor));
    if (!exec) return NULL;

    exec->threads = malloc(MAX_THREADS * sizeof(pthread_t));
    if (!exec->threads) {
        free(exec);
        return NULL;
    }

    queue_init(&exec->queue);
    pthread_mutex_init(&exec->mutex, NULL);
    pthread_cond_init(&exec->not_empty, NULL);
    pthread_cond_init(&exec->not_full, NULL);

    exec->running = true;
    exec->active_tasks = 0;
    exec->thread_count = MAX_THREADS;

    for (size_t i = 0; i < exec->thread_count; i++) {
        if (pthread_create(&exec->threads[i], NULL, executor_worker, exec) != 0) {
            executor_shutdown(exec);
            return NULL;
        }
    }

    return exec;
}

bool executor_submit(ThreadExecutor *exec, void (*func)(void*), void *arg) {
    if (!exec || !func) return false;

    pthread_mutex_lock(&exec->mutex);

    while (exec->queue.count >= EXECUTOR_QUEUE_SIZE && exec->running) {
        pthread_cond_wait(&exec->not_full, &exec->mutex);
    }

    if (!exec->running) {
        pthread_mutex_unlock(&exec->mutex);
        return false;
    }

    bool success = queue_push(&exec->queue, func, arg);
    if (success) {
        pthread_cond_signal(&exec->not_empty);
    }

    pthread_mutex_unlock(&exec->mutex);
    return success;
}

void executor_shutdown(ThreadExecutor *exec) {
    if (!exec) return;

    pthread_mutex_lock(&exec->mutex);
    exec->running = false;
    pthread_cond_broadcast(&exec->not_empty);
    pthread_mutex_unlock(&exec->mutex);

    for (size_t i = 0; i < exec->thread_count; i++) {
        pthread_join(exec->threads[i], NULL);
    }

    while (exec->queue.head) {
        ExecutorTask *task = queue_pop(&exec->queue);
        free(task);
    }

    pthread_mutex_destroy(&exec->mutex);
    pthread_cond_destroy(&exec->not_empty);
    pthread_cond_destroy(&exec->not_full);

    free(exec->threads);
    free(exec);
}
