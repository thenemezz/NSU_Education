#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netdb.h>
#include <ctype.h>
#include "../header/proxy.h"
#define PORT            8080
#define QUEUE_SIZE      30

#define COLOR_RED   "\033[31m"
#define COLOR_BLUE  "\033[34m"
#define RESET       "\033[0m"

Proxy *proxy;

void finish_proxy() {
    proxy->is_active = 0;
}

int createAndBindServerSocket() {
    int reuse = 1;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf(COLOR_RED "ERROR: create server socket\n" RESET);
        return -1;
    }
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        fprintf(stderr, COLOR_RED "ERROR: Failed to set socket options\n" RESET);
        close(server_socket);
        return -2;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int err = bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (err) {
        printf(COLOR_RED "ERROR: Failed to bind serv socket\n" RESET);
        close(server_socket);
        return -3;
    }

    return server_socket;
}

int setupSignalHandlers() {
    struct sigaction act;
    act.sa_handler = finish_proxy;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(SIGINT, &act, NULL) == -1) {
        printf(COLOR_RED "ERROR: Failed to install SIGINT handler\n" RESET);
        return -1;
    }
    if (sigaction(SIGQUIT, &act, NULL) == -1) {
        printf(COLOR_RED "ERROR: Failed to install SIGQUIT handler\n" RESET);
        return -1;
    }
    struct sigaction ignore_act;
    ignore_act.sa_handler = SIG_IGN;
    sigemptyset(&ignore_act.sa_mask);
    ignore_act.sa_flags = 0;

    if (sigaction(SIGPIPE, &ignore_act, NULL) == -1) {
        printf(COLOR_RED "ERROR: Failed to ignore SIGPIPE\n" RESET);
        return -1;
    }

    return 0;
}

Proxy *initializeProxy(Cache *cache, ThreadExecutor *threadpool) {
    if (setupSignalHandlers() != 0) {
        printf("ERROR: Failed to setup signal handlers\n");
        return NULL;
    }
    printf(COLOR_BLUE "INFO: Signal handlers for SIGINT and SIGQUIT setup.\n" RESET);

    Proxy *proxy = malloc(sizeof(*proxy));
    if (!proxy) {
        printf("ERROR: initializeProxy: malloc failed\n");
        return NULL;
    }

    proxy->server_socket = createAndBindServerSocket();
    if (proxy->server_socket == -1) {
        free(proxy);
        return NULL;
    }

    proxy->cache = cache;
    proxy->threadpool = threadpool;
    proxy->is_active = 1;

    return proxy;
}

int start_proxy(Proxy *proxy) {
    int err = listen(proxy->server_socket, QUEUE_SIZE);
    if (err)
        return -1;

    printf(COLOR_BLUE "Proxy started\n" RESET);

    while (proxy->is_active) {
        int clientSocket = accept(proxy->server_socket, NULL, NULL);
        if (clientSocket < 0) {
            printf(COLOR_RED "ERROR: accepting connection: %s\n" RESET, strerror(errno));
            continue;
        }

        printf(COLOR_BLUE "Accepted new connection\n" RESET);

        clientHandlerArgs_t *args = malloc(sizeof(*args));
        if (!args) {
            printf(COLOR_RED "ERROR: handler args\n" RESET);
            shutdown(clientSocket, SHUT_RDWR);
            close(clientSocket);
            continue;
        }

        args->sockToClient = clientSocket;
        args->cache = proxy->cache;
        executor_submit(proxy->threadpool, handleClientRequest, args);
    }

    return 0;
}

int sendData(int sock, char *data, size_t len) {
    size_t total_sent = 0;
    ssize_t bytes_sent;

    while (total_sent < len) {
        bytes_sent = write(sock, data + total_sent, len - total_sent);

        if (bytes_sent == -1) {
            if (errno == EINTR) {
                printf(COLOR_RED "ERROR: write interrupted by signal\n" RESET);
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf(COLOR_RED "ERROR: Socket temporarily unavailable, retrying in 1 second...\n" RESET);
                sleep(1);
                continue;
            } else {
                printf(COLOR_RED "ERROR: write failed: %s\n" RESET, strerror(errno));
                return -1;
            }
        }

        total_sent += bytes_sent;
    }

    return 0;
}

int sendCachedDataToClient(int sock, Node *cache) {
    int err;
    int result;
    size_t total_sent = 0;
    size_t bytes_sent;

    pthread_mutex_lock(&cache->dataLock);

    while (1) {
        while (!cache->isFinalized && cache->currentSize == total_sent) {
            pthread_cond_wait(&cache->conditionVar, &cache->dataLock);
        }

        bytes_sent = cache->currentSize - total_sent;
        err = sendData(sock, cache->dataBuffer + total_sent, bytes_sent);
        if (err) {
            result = -1;
            break;
        }
        total_sent = cache->currentSize;

        if (cache->isFinalized) {
            result = 0;
            break;
        }
    }

    pthread_mutex_unlock(&cache->dataLock);
    return result;
}

int connectServer(const char *hostName) {
    struct addrinfo *addrInfo;
    int err;

    int sockToServ = socket(AF_INET, SOCK_STREAM, 0);
    if (!sockToServ) {
        printf(COLOR_RED "ERROR: Failed to create new socket\n" RESET);
        return -1;
    }

    struct timeval socketTimeout;
    socketTimeout.tv_sec = 10;
    socketTimeout.tv_usec = 0;

    int rcv = setsockopt(sockToServ, SOL_SOCKET, SO_RCVTIMEO, &socketTimeout, sizeof(socketTimeout));
    int snd = setsockopt(sockToServ, SOL_SOCKET, SO_SNDTIMEO, &socketTimeout, sizeof(socketTimeout));
    if (rcv || snd) {
        printf(COLOR_RED "ERROR: Failed to set socket timeout\n" RESET);
    }

    err = getaddrinfo(hostName, "http", NULL, &addrInfo);
    if (err) {
        printf(COLOR_RED "ERROR: Failed in get serv addr\n" RESET);
        close(sockToServ);
        return -1;
    }

    err = connect(sockToServ, addrInfo->ai_addr, addrInfo->ai_addrlen);
    if (err) {
        printf(COLOR_RED "ERROR: connectToServ Failed to connect: %s\n" RESET, strerror(errno));
        close(sockToServ);
        freeaddrinfo(addrInfo);
        return -1;
    }

    freeaddrinfo(addrInfo);
    return sockToServ;
}

ssize_t readAndParseRequest(int sock, char *buf, size_t maxLen, httpRequest *parseData) {
    int err;
    ssize_t rret;
    size_t buflen = 0, prevbuflen = 0;

    while (1) {
        while ((rret = read(sock, buf + buflen, maxLen - buflen)) == -1 && errno == EINTR);
        if (rret == -1) {
            printf(COLOR_RED "ERROR: server disconnect: %s\n" RESET, strerror(errno));
            return -1;
        }
        if (rret == 0) {
            printf(COLOR_BLUE "Socket shutdown\n" RESET);
            return -1;
        }

        prevbuflen = buflen;
        buflen += rret;

        parseData->numHeaders = sizeof(parseData->headers) / sizeof(parseData->headers[0]);
        err = parse_request(buf, buflen, parseData);

        if (err > 0) {
            break;
        }

        if (err < 0) {
            printf(COLOR_RED "ERROR: parsing request\n" RESET);
            return -1;
        }

    }

    return buflen;
}

ssize_t readAndParseResponse(int sockToServ, int sockToClient, char *req, Cache *cacheStorage, int *status) {
    httpResponse parse;
    Node *newCache = NULL;
    ssize_t recvd;
    size_t recvdTotal = 0, headerLen, contentLen;
    char buf[BUFFER_SIZE + 1];
    char *headerEnd;
    int err;

    // Считываем, пока не найдём \r\n\r\n (заголовки ответа)
    do {
        recvd = read(sockToServ, buf + recvdTotal, BUFFER_SIZE - recvdTotal);
        if (recvd < 0) {
            printf(COLOR_RED "ERROR: Receive error: %s\n" RESET, strerror(errno));
            return -1;
        }
        if (recvd == 0) {
            printf(COLOR_RED "ERROR: server disconnect: \n" RESET);
            return -1;
        }

        recvdTotal += recvd;
        buf[recvdTotal] = '\0';
    } while ((headerEnd = strstr(buf, "\r\n\r\n")) == NULL);

    headerEnd += strlen("\r\n\r\n");
    headerLen = headerEnd - buf;

    parse.numHeaders = sizeof(parse.headers) / sizeof(parse.headers[0]);
    err = parse_response(buf, headerLen, &parse);
    if (err < 0) {
        printf(COLOR_RED "ERROR: parsing response: %i\n" RESET, err);
        return -1;
    }
    *status = parse.status;

    contentLen = parse.contentLength;

    err = sendData(sockToClient, buf, recvdTotal);
    if (err) {
        printf(COLOR_RED "ERROR: Failed to send data back to client: \n" RESET);
        return -1;
    }

    // обрабатываем статус 200
    if (parse.status == 200) {
        newCache = createCacheNode();
        if (!newCache) {
            printf(COLOR_RED "ERROR: Failed to create new cache node. \n" RESET);
        }
    }

    // Записываем уже полученные данные в кэш
    err = appendToCacheNode(newCache, buf, recvdTotal);
    if (err) {
        printf(COLOR_RED "ERROR: Failed to append data to cache node. \n" RESET);
        destroyCacheNode(newCache);
        newCache = NULL;
    }

    err = addToCache(cacheStorage, req, newCache);
    if (err) {
        printf(COLOR_RED "ERROR: Failed to add cache node to storage. \n" RESET);
        destroyCacheNode(newCache);
        newCache = NULL;
    }

    ssize_t remaining = headerLen + contentLen - recvdTotal;
    while (remaining > 0) {
        recvd = read(sockToServ, buf, BUFFER_SIZE);
        if (recvd <= 0) {
            printf(COLOR_RED "ERROR: Server disconnect: %s \n" RESET, strerror(errno));
            removeCacheNode(cacheStorage, req);
            return -1;
        }

        remaining   -= recvd;
        recvdTotal  += recvd;

        err = sendData(sockToClient, buf, recvd);
        if (err) {
            printf(COLOR_RED "ERROR: Failed to send data back to client. \n" RESET);
            removeCacheNode(cacheStorage, req);
            return -1;
        }

        err = appendToCacheNode(newCache, buf, recvd);
        if (err) {
            printf(COLOR_RED "ERROR: Failed to append data to cache node. \n" RESET);
            removeCacheNode(cacheStorage, req);
            newCache = NULL;
        }
    }

    finalizeCacheNode(newCache);
    printf(COLOR_BLUE "Cached response to %s\n" RESET, req);
    return recvdTotal;
}

void handleClientRequest(void *args) {
    struct timeval socketTimeout;
    int sockToClient = ((clientHandlerArgs_t*) args)->sockToClient;
    Cache *cacheStorage = ((clientHandlerArgs_t*) args)->cache;

    socketTimeout.tv_sec = 10;
    socketTimeout.tv_usec = 0;

    int rcv = setsockopt(sockToClient, SOL_SOCKET, SO_RCVTIMEO, &socketTimeout, sizeof(socketTimeout));
    int snd = setsockopt(sockToClient, SOL_SOCKET, SO_SNDTIMEO, &socketTimeout, sizeof(socketTimeout));

    if (rcv || snd) {
        printf(COLOR_RED "ERROR: Failed to set socket timeout\n" RESET);
    }

    char request[8192 + 1] = {'\0'};
    httpRequest reqParse;
    Node *cacheNode = NULL;
    ssize_t reqLen, respLen;
    int sockToServ;
    int err;

    reqLen = readAndParseRequest(sockToClient, request, 8192, &reqParse);
    if (reqLen < 0) {
        printf(COLOR_RED "ERROR: Failed to read client request \n" RESET);
        close(sockToClient);
        return;
    }

    size_t pathLen = strlen(reqParse.path);
    char path[pathLen + 1];
    memcpy(path, reqParse.path, pathLen);
    path[pathLen] = '\0';

    if (strncmp(reqParse.method, "GET", 3) != 0) {
        printf(COLOR_RED "ERROR: use only GET \n" RESET);
        close(sockToClient);
        return;
    }

    // ищем в кэше
    cacheNode = getCachedNode(cacheStorage, path);
    if (cacheNode) {
        printf(COLOR_BLUE "Found cache node for resource %s\n" RESET, path);

        err = sendCachedDataToClient(sockToClient, cacheNode);
        if (err) {
            printf(COLOR_RED "ERROR: Failed to send response to client \n" RESET);
        }

        close(sockToClient);
        return;
    }

    printf(COLOR_BLUE "Cache node for resource %s not found\n" RESET, path);

    if (!reqParse.host) {
        printf(COLOR_RED "ERROR: Host header not found\n" RESET);
        close(sockToClient);
        return;
    }

    // kоннект к серверу
    sockToServ = connectServer(reqParse.host);
    if (sockToServ < 0) {
        printf(COLOR_RED "ERROR: Failed to connect to %s\n" RESET, reqParse.host);
        close(sockToClient);
        return;
    }
    printf(COLOR_BLUE "Connected to server %s\n" RESET, reqParse.host);

    // клиентский запрос на сервер
    err = sendData(sockToServ, request, reqLen);
    if (err) {
        printf(COLOR_RED "ERROR: Failed to connect:  %s\n" RESET, strerror(errno));
        close(sockToClient);
        close(sockToServ);
        return;
    }
    printf(COLOR_BLUE "Data sent to server %s successfully\n" RESET, reqParse.host);

    int status;
    respLen = readAndParseResponse(sockToServ, sockToClient, path, cacheStorage, &status);
    if (respLen < 0) {
        printf(COLOR_RED "ERROR: Error handling response.\n" RESET);
        close(sockToClient);
        close(sockToServ);
        return;
    }
    printf(COLOR_BLUE "Server %s responded with %ld bytes, status: %d\n" RESET, reqParse.host, respLen, status);

    free(args);
    close(sockToClient);
    close(sockToServ);
}

int run_proxy() {
    Cache *cache = createCache();
    if (cache == NULL) {
        printf(COLOR_RED "ERROR: Fail in create cache\n" RESET);
        return -1;
    }

    ThreadExecutor *threadpool = executor_create();
    if (!threadpool) {
        printf(COLOR_RED "ERROR: Fail in create threadpool\n" RESET);
        exit(EXIT_FAILURE);
    }

    proxy = initializeProxy(cache, threadpool);
    if (!proxy) {
        printf(COLOR_RED "ERROR:  creating proxy\n" RESET);
        exit(EXIT_FAILURE);
    }

    int err = start_proxy(proxy);
    if (err) {
        printf(COLOR_RED "ERROR: starting proxy\n" RESET);
        exit(EXIT_FAILURE);
    }

    if (!proxy) {
        close(proxy->server_socket);
        free(proxy);
    }
    executor_shutdown(threadpool);
    destroyCache(cache);

    return 0;
}
