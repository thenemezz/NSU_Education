#ifndef __FITOS_QUEUE_H__
#define __FITOS_QUEUE_H__

#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


// Узел очереди
typedef struct _QueueNode {
    int val;
    struct _QueueNode *next;
} qnode_t;


// Очередь
typedef struct _Queue {
    qnode_t *first;
    qnode_t *last;
    // поток для отслеживания очереди
    pthread_t qmonitor_tid;
    pthread_spinlock_t lock;

    int count;
    int max_count;

    // попаток добавить / извлечь
    long add_attempts;
    long get_attempts;
    long add_count;
    long get_count;
} queue_t;

queue_t *queue_init(int max_count);
void queue_destroy(queue_t *q);
int queue_add(queue_t *q, int val);
int queue_get(queue_t *q, int *val);
void queue_print_stats(queue_t *q);

#endif  // __FITOS_QUEUE_H__
