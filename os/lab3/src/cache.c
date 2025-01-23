#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdatomic.h>
#include "../header/cache.h"

#define COLOR_RED "\033[31m"
#define COLOR_BLUE "\033[34m"
#define COLOR_GREEN "\033[32m"
#define RESET "\033[0m"

static void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, COLOR_RED "Memory allocation failed\n" RESET);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static void *safe_realloc_func(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (!new_ptr) {
        fprintf(stderr, COLOR_RED "Memory reallocation failed\n" RESET);
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}

static char *safe_strdup_func(const char *str) {
    char *dup = strdup(str);
    if (!dup) {
        fprintf(stderr, COLOR_RED "String duplication failed\n" RESET);
        exit(EXIT_FAILURE);
    }
    return dup;
}

Node *createCacheNode(void) {
    Node *node = (Node *)safe_malloc(sizeof(Node));
    memset(node, 0, sizeof(Node));

    if (pthread_mutex_init(&node->dataLock, NULL) != 0 || pthread_cond_init(&node->conditionVar, NULL) != 0) {
        fprintf(stderr, COLOR_RED "Failed to initialize synchronization primitives\n" RESET);
        free(node);
        return NULL;
    }

    atomic_init(&node->usageCount, 1);
    node->request = NULL;

    node->dataBuffer = (char *)safe_malloc(16384);
    if (!node->dataBuffer) {
        fprintf(stderr, COLOR_RED "Failed to allocate initial memory for dataBuffer\n" RESET);
        pthread_mutex_destroy(&node->dataLock);
        pthread_cond_destroy(&node->conditionVar);
        free(node);
        return NULL;
    }
    node->currentSize = 0;
    node->maxCapacity = 16384;
    node->isFinalized = 0;

    printf(COLOR_BLUE "createCacheNode: created %p with initial dataBuffer of 16384 bytes\n" RESET, (void*)node);
    return node;
}

void destroyCacheNode(Node *node) {
    if (!node) return;

    pthread_mutex_lock(&node->dataLock);
    free(node->dataBuffer);
    free(node->request);
    pthread_mutex_unlock(&node->dataLock);

    pthread_mutex_destroy(&node->dataLock);
    pthread_cond_destroy(&node->conditionVar);
    free(node);
}

int appendToCacheNode(Node *node, const char *newData, size_t size) {
    if (!newData || !node || size == 0) return -1;

    pthread_mutex_lock(&node->dataLock);

    size_t new_size = node->currentSize + size;

    if (new_size >= node->maxCapacity) {
        size_t new_capacity = (node->maxCapacity > 0) ? node->maxCapacity * 2 : new_size * 2 + 4096;

        char *new_buf = (char *)safe_realloc_func(node->dataBuffer, new_capacity);
        node->dataBuffer = new_buf;
        node->maxCapacity = new_capacity;
    }

    memcpy(node->dataBuffer + node->currentSize, newData, size);
    node->currentSize = new_size;

    pthread_cond_broadcast(&node->conditionVar);
    pthread_mutex_unlock(&node->dataLock);
    return 0;
}


void finalizeCacheNode(Node *node) {
    if (!node) return;
    pthread_mutex_lock(&node->dataLock);
    node->isFinalized = 1;
    pthread_cond_broadcast(&node->conditionVar);
    pthread_mutex_unlock(&node->dataLock);
}

Cache *createCache(void) {
    Cache *storage = (Cache *)safe_malloc(sizeof(Cache));
    memset(storage->nodes, 0, sizeof(storage->nodes));

    if (pthread_mutex_init(&storage->cacheLock, NULL) != 0) {
        fprintf(stderr, COLOR_RED "Failed to initialize cache mutex\n" RESET);
        free(storage);
        return NULL;
    }

    printf(COLOR_BLUE "Cache initialized with size %d!\n" RESET, CACHE_SIZE);
    return storage;
}

void destroyCache(Cache *storage) {
    if (!storage) return;

    pthread_mutex_lock(&storage->cacheLock);

    for (int i = 0; i < CACHE_SIZE; i++) {
        if (storage->nodes[i]) {
            destroyCacheNode(storage->nodes[i]);
            storage->nodes[i] = NULL;
        }
    }

    pthread_mutex_unlock(&storage->cacheLock);
    pthread_mutex_destroy(&storage->cacheLock);
    free(storage);

    printf(COLOR_GREEN "Cache destroyed.\n" RESET);
}

//поиск номера ноды по URL [i] -нашли 0 -нет
static int findNodeIndex(Cache *storage, const char *req, int *foundIndex) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (storage->nodes[i] && strcmp(storage->nodes[i]->request, req) == 0) {
            if (foundIndex) {
                *foundIndex = i;
            }
            return 1;
        }
    }
    return 0;
}

static int findFreeOrLRUIndex(Cache *storage) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (storage->nodes[i] == NULL) {
            return i;
        }
    }

    // нет свободных мест, ищем LRU узел
    unsigned int minUsage = 999999;
    int lruIndex = -1;
    for (int i = 0; i < CACHE_SIZE; i++) {
        unsigned int usage = atomic_load(&storage->nodes[i]->usageCount);
        if (usage < minUsage) {
            minUsage = usage;
            lruIndex = i;
        }
    }
    return lruIndex;
}

// Добавление узла в кэш с учетом LRU
int addToCache(Cache *storage, const char *req, Node *node) {
    if (!storage || !req || !node) return -1;

    pthread_mutex_lock(&storage->cacheLock);

    int existingIndex;

    // URL уже существует в кэше
    if (findNodeIndex(storage, req, &existingIndex)) {
        atomic_fetch_add(&storage->nodes[existingIndex]->usageCount, 1);
        printf(COLOR_BLUE "addToCache: URL '%s' already in cache. Incremented usageCount to %u.\n" RESET,
               req, atomic_load(&storage->nodes[existingIndex]->usageCount));
        pthread_mutex_unlock(&storage->cacheLock);
        destroyCacheNode(node);
        return 0;
    }

    int index = findFreeOrLRUIndex(storage);
    if (index == -1) {
        fprintf(stderr, COLOR_RED "addToCache: Failed to find a slot for the new node\n" RESET);
        pthread_mutex_unlock(&storage->cacheLock);
        return -1;
    }

    //место занято, удалить LRU узел
    if (storage->nodes[index] != NULL) {
        printf(COLOR_BLUE "addToCache: Cache is full. Removing LRU node with URL '%s' at index %d\n" RESET,
               storage->nodes[index]->request, index);
        destroyCacheNode(storage->nodes[index]);
    }

    // заменяем узел
    storage->nodes[index] = node;
    node->request = safe_strdup_func(req);
    atomic_store(&node->usageCount, 1);

    printf(COLOR_BLUE "addToCache: Added URL '%s' to cache at index %d with usageCount = 1\n" RESET, req, index);
    pthread_mutex_unlock(&storage->cacheLock);
    return 0;
}

//получение узла из кэша и обновление его usageCount
Node *getCachedNode(Cache *storage, const char *req) {
    if (!storage || !req) return NULL;

    pthread_mutex_lock(&storage->cacheLock);

    for (int i = 0; i < CACHE_SIZE; i++) {
        if (storage->nodes[i] && strcmp(storage->nodes[i]->request, req) == 0) {
            atomic_fetch_add(&storage->nodes[i]->usageCount, 1);
            printf(COLOR_BLUE "getCachedNode: Found URL '%s' in cache at index %d. Incremented usageCount to %u.\n" RESET,
                   req, i, atomic_load(&storage->nodes[i]->usageCount));
            pthread_mutex_unlock(&storage->cacheLock);
            return storage->nodes[i];
        }
    }

    pthread_mutex_unlock(&storage->cacheLock);
    printf(COLOR_BLUE "getCachedNode: URL '%s' not found in cache\n" RESET, req);
    return NULL;
}

int removeCacheNode(Cache *storage, const char *req) {
    if (!storage || !req) return -1;

    pthread_mutex_lock(&storage->cacheLock);

    for (int i = 0; i < CACHE_SIZE; i++) {
        if (storage->nodes[i] && strcmp(storage->nodes[i]->request, req) == 0) {
            printf(COLOR_BLUE "removeCacheEntry: Removing URL '%s' from cache at index %d\n" RESET, req, i);
            destroyCacheNode(storage->nodes[i]);
            storage->nodes[i] = NULL;
            pthread_mutex_unlock(&storage->cacheLock);
            return 0;
        }
    }

    pthread_mutex_unlock(&storage->cacheLock);
    printf(COLOR_BLUE "removeCacheEntry: URL '%s' not found in cache\n" RESET, req);
    return -1;
}
