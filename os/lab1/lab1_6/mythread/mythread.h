#include <stdint.h>
#include <stddef.h>

// Определения макросов
#define PAGE_SIZE 4096
#define PAGE_GUARD 4
#define MEM_SIZE (PAGE_SIZE * 1024) // 4 Мб
#define NUM_THREADS 10

#define TRUE 1
#define FALSE 0

// Определение структуры mythread_t
typedef struct mythread_t {
    uint64_t thread_id;
} mythread_t;

// Определение структуры mythread_data
typedef struct mythread_data {
    mythread_t thread_id;
    size_t mem_size;
    void* stack_ptr;
    void* (*func)(void*);
    void* arg;
    void* result;
    volatile char is_joined;
    volatile char is_finished;
} mythread_data;

// Функции
int mythread_create(mythread_t* thread, void* (*func)(void*), void* arg);
int mythread_join(mythread_t thread, void** retval);

