#ifndef __FITOS_QUEUE_H__
#define __FITOS_QUEUE_H__

#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _QueueNode {
    int val;
    struct _QueueNode *next;
} qnodeT;

typedef struct _Queue {
    qnodeT *first;
    qnodeT *last;

    pthread_t qmonitorTid;

    int count; // amount of elements in a current moment
    int maxCount;

    // queue statistics
    long addAttempts; // типа сколько попыток было сделано, чтоб
    // запистать элемент
    long getAttempts; // количество попыток прочитать элемент (не факт,
    // что смогли прочитать из очереди, тк она может
    // быть пустой вообще, например)
    long addCount; // сколько из этих попыток было успешных, то есть
    // вообще сколько добавли элементов
    long getCount; // сколько прочитали элементов

    pthread_spinlock_t lock; // 1 - можно залочить

} queueT;

queueT *queueInit(int maxCount);
void queueDestroy(queueT *q);
int queueAdd(queueT *q, int val);
int queueGet(queueT *q, int *val);
void queuePrintStats(queueT *q);

#endif // __FITOS_QUEUE_H__