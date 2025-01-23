#include "../header/cache.h"

typedef struct {
    const char *name;
    const char *value;
    size_t name_len;
    size_t value_len;
} httpHeader_t;

typedef struct {
    const char *method;
    const char *path;
    const char *host;
    httpHeader_t headers[20];
    size_t numHeaders;
} httpRequest;

typedef struct {
    int status;
    size_t contentLength;
    httpHeader_t headers[20];
    size_t numHeaders;
} httpResponse;

typedef struct {
    int sockToClient;
    Cache *cache;
} clientHandlerArgs_t;

ssize_t parse_request(const char *buf, size_t buflen, httpRequest *parseData);

ssize_t parse_response(const char *buf, size_t buflen, httpResponse *resp);