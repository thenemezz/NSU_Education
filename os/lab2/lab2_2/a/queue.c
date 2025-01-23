#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>

#include "queue.h"

void createSpinlock(queueT *q) {
  if (pthread_spin_lock(&q->lock)) {
    printf("pthread_spin_lock() error: %s\n", strerror(errno));
    abort();
  }
}

void destroySpinlock(queueT *q) {
  if (pthread_spin_unlock(&q->lock)) {
    printf("pthread_spin_unlock() error: %s\n", strerror(errno));
    abort();
  }
}

void *qmonitor(void *arg) {
  queueT *q = (queueT *)arg;

  printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

  while (1) {
    queuePrintStats(q);
    sleep(1);
  }

  return NULL;
}

queueT *queueInit(int maxCount) {
  queueT *q = malloc(sizeof(queueT)); // malloc mem for structure
  if (!q) {
    printf("Cannot allocate memory for a queue\n");
    free(q);
    abort();
  }

  q->first = NULL;
  q->last = NULL;
  q->maxCount = maxCount;
  q->count = 0;

  q->addAttempts = q->getAttempts = 0;
  q->addCount = q->getCount = 0;

  int errSpintInit =
      pthread_spin_init(&q->lock, PTHREAD_PROCESS_PRIVATE);
  /*  PTHREAD_PROCESS_PRIVATE
  разрешает использовать спин-блокировку только потокам, созданным в
  том же процессе, что и поток, который инициализировал
  спин-блокировку.
  */
  if (errSpintInit) {
    printf("queueInit: pthread_spin_init() failed: %s\n",
           strerror(errSpintInit));
    free(q);
    abort();
  }

  int err = pthread_create(&q->qmonitorTid, NULL, qmonitor, q);
  if (err) {
    printf("queueInit: pthread_create() failed: %s\n",
           strerror(err));
    free(q);
    abort();
  }

  return q;
}

void queueDestroy(queueT *q) {
  const int errCancel = pthread_cancel(q->qmonitorTid);
  if (errCancel) {
    printf("queueDestroy: pthread_cancel() error");
  }

  int errDestroySpin = pthread_spin_destroy(&q->lock);
  if (errDestroySpin) {
    printf("queueDestroy: pthread_spin_destroy() error : %s\n",
           strerror(errno));
  }

  qnodeT *currNode = q->first;
  while (currNode != NULL) {
    qnodeT *tmp = currNode;
    currNode = currNode->next;
    free(tmp);
  }
  free(q);
}

int queueAdd(queueT *q, int val) {
  createSpinlock(q);

  q->addAttempts++; // +1 попытка записать элемент

  assert(q->count <= q->maxCount);

  if (q->count == q->maxCount) {
    destroySpinlock(q);
    return 0;
  }

  qnodeT *new = malloc(sizeof(qnodeT)); // malloc mem for one node
  if (!new) {
    printf("Cannot allocate memory for new node\n");
    free(new);
    abort();
  }

  new->val = val;
  new->next = NULL;

  if (!q->first) { // only one 1st node in queue
    q->first = q->last = new;
  } else { // not the 1st node in queue
    q->last->next = new;
    q->last = q->last->next;
  }

  q->count++; // количество элементов на текущий момент
  q->addCount++; // сколько добавили элементов

  destroySpinlock(q);

  return 1;
}

int queueGet(queueT *q, int *val) {
  createSpinlock(q);

  assert(q->count >= 0);

  if (q->count == 0) {
    destroySpinlock(q);
    return 0;
  }

  qnodeT *tmp = q->first;

  *val = tmp->val;
  q->first = q->first->next;

  free(tmp);
  q->count--;
  q->getCount++;

  destroySpinlock(q);

  return 1;
}

void queuePrintStats(queueT *q) {
  // here we print amount of attempts и how many of them are lucky
  createSpinlock(q);

  int count = q->count;
  long addAttempts = q->addAttempts;
  long getAttempts = q->getAttempts;
  long addCount = q->addCount;
  long getCount = q->getCount;

  destroySpinlock(q);

  printf("\n");
  printf("queue stats: current size %d;\n", count);
  printf("attempts: (addAttempts: %ld; getAttempts: %ld; "
         "addAttempts - getAttempts: %ld)\n",
         addAttempts, getAttempts, addAttempts - getAttempts);
  printf("counts: (addCount: %ld; getCount: %ld; addCount - "
         "getCount: %ld)\n",
         addCount, getCount, addCount - getCount);
}