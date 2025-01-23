#pragma once
#include <sys/socket.h>
#include "http_request.h"
#include "cache.h"
#include "threadpool.h"

#define BUFFER_SIZE 4096

typedef struct {
    int is_active;
    int server_socket;
    Cache *cache;
    ThreadExecutor *threadpool;
} Proxy;

int start_proxy(Proxy *proxy);

void finish_proxy();

int createAndBindServerSocket();

int setupSignalHandlers();

void handleClientRequest(void *args);

Proxy *initializeProxy(Cache *cache, ThreadExecutor *threadpool);

int sendData(int sock, char *data, size_t len);

int sendCachedDataToClient(int sock, Node *cache);

int connectServer(const char *hostName);

ssize_t readAndParseRequest(int sock, char *buf, size_t maxLen, httpRequest *parseData);

ssize_t readAndParseResponse(int sockToServ, int sockToClient, char *req, Cache *cacheStorage, int *status);

int run_proxy();