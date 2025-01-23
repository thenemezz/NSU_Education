#pragma once

#include <pthread.h>
#include <stddef.h>
#include <stdatomic.h>

#define MAX_THREADS 20
#define MAX_QUEUE_SIZE 100
#define CACHE_SIZE 100

typedef struct Node_ {
    atomic_uint usageCount;
    char *request;
    char *dataBuffer;
    size_t currentSize;
    size_t maxCapacity;

    int isFinalized;

    pthread_mutex_t dataLock;
    pthread_cond_t conditionVar;
} Node;

typedef struct {
    Node *nodes[CACHE_SIZE];
    pthread_mutex_t cacheLock;
} Cache;


Cache *createCache(void);

void destroyCache(Cache *storage);

Node *createCacheNode(void);

void destroyCacheNode(Node *node);

int appendToCacheNode(Node *node, const char *newData, size_t size);

void finalizeCacheNode(Node *node);

int addToCache(Cache *storage, const char *req, Node *node);

Node *getCachedNode(Cache *storage, const char *req);

int removeCacheNode(Cache *storage, const char *req);
