#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ucontext.h>
#include <sched.h>
#include <linux/sched.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "mythread.h" // Включение заголовочного файла без защитных директив

// была ли инициализирована глобальная структура для потоков
char INITED = FALSE;

mythread_t THREADS[NUM_THREADS];

static mythread_data* id2data(mythread_t thread_id) {
    mythread_data* data;
    size_t offset = (MEM_SIZE + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE - sizeof(mythread_data);
    return (mythread_data*) (thread_id.thread_id + offset);
}

static mythread_data* init_thread_stack() {
    if (INITED == FALSE) {//inited
        INITED = TRUE;
        for (int i = 0; i < NUM_THREADS; i++) {
            THREADS[i].thread_id = 0;
        }
    }

    int stack_index = -1;

    for (int i = 0; i < NUM_THREADS; ++i) {
        if (THREADS[i].thread_id == 0) { // проверяет два раза
            stack_index = i;
            break;
        } else {
            mythread_data* data = id2data(THREADS[i]);
            if (data->is_joined) {
                data->stack_ptr = (void*)data->thread_id.thread_id + data->mem_size - PAGE_SIZE;
                return data;
            }
        }
    }

    // Свободный стек не найден
    if (stack_index == -1) {
        printf("No available thread stack\n");
        return NULL;
    }

    // Если свободный стек найден, создается новая структура mythread_data
    mythread_data data;

    void* mem_ptr;

    data.mem_size = (MEM_SIZE + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE;

    // Адрес начала выделенной памяти
    mem_ptr = mmap(
        NULL,
        data.mem_size,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );

    if (mem_ptr == MAP_FAILED) {
        printf("%s\n", strerror(errno));
        errno = 0;
        return NULL;
    }

    // PAGE GUARD
    if (mprotect(mem_ptr, PAGE_SIZE * PAGE_GUARD, PROT_NONE)) {
        printf("%s\n", strerror(errno));
        if (munmap(mem_ptr, data.mem_size)) {
            printf("%s\n", strerror(errno));
        }
        errno = 0;
        return NULL;
    }

    data.thread_id.thread_id = (uint64_t) mem_ptr;
    // Указатель на вершину стека
    data.stack_ptr = mem_ptr + data.mem_size - PAGE_SIZE;

    // Копируем содержимое структуры data в выделенную область памяти в стеке потока.
    memcpy(data.stack_ptr + PAGE_SIZE - sizeof(data), &data, sizeof(data));

    THREADS[stack_index] = data.thread_id;

    return id2data(data.thread_id);
}

static int mythread_init(void* arg) {
    mythread_data* thread = (mythread_data*) arg;

    thread->result = thread->func(thread->arg);
    thread->is_finished = TRUE;

    while (thread->is_joined == FALSE) {
        sleep(1);
    }

    return 0;
}

int mythread_create(mythread_t* thread, void* (*func)(void*), void* arg) {
    mythread_data* thread_data = init_thread_stack();

    if (thread_data == NULL) {
        return 1;
    }

    thread_data->func = func;
    thread_data->arg = arg;
    thread_data->result = NULL;
    thread_data->is_joined = FALSE;
    thread_data->is_finished = FALSE;

    int clone_result = clone(
        mythread_init,
        thread_data->stack_ptr,
        CLONE_VM | CLONE_PARENT |
        CLONE_FS | CLONE_FILES |
        CLONE_SIGHAND | CLONE_THREAD |
        CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID,
        (void*) thread_data
    );

    if (clone_result == -1) {
        fprintf(stderr, "Failed to create thread: %s\n", strerror(errno));
        errno = 0;
        return 1;
    }

    thread->thread_id = thread_data->thread_id.thread_id;

    return 0;
}

int mythread_join(mythread_t thread, void** retval) {
    mythread_data* thread_data = id2data(thread);

    if (thread_data == NULL) {
        return 1;
    }

    while (thread_data->is_finished == FALSE) {
        sleep(1);
    }

    *retval = thread_data->result;
    thread_data->is_joined = TRUE;

    return 0;
}

