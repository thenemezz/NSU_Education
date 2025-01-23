#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../header/http_request.h"

ssize_t parse_request(const char *buf, size_t buflen, httpRequest *parseData) {
    const char *endOfHeaders = strstr(buf, "\r\n\r\n");
    if (!endOfHeaders) {
        return 0;
    }

    // Парсим первую строку
    const char *lineEnd = strstr(buf, "\r\n");
    if (!lineEnd) {
        return -1;
    }
    size_t requestLineLen = (size_t)(lineEnd - buf);

    // Копируем строку запроса во временный буфер
    char *requestLine = (char*)malloc(requestLineLen + 1);
    if (!requestLine) return -1;
    memcpy(requestLine, buf, requestLineLen);
    requestLine[requestLineLen] = '\0';

    char *methodStr  = strtok(requestLine, " ");
    char *pathStr    = strtok(NULL, " ");
    char *versionStr = strtok(NULL, " ");

    if (!methodStr || !pathStr || !versionStr) {
        free(requestLine);
        return -1;
    }

    parseData->method = methodStr;
    parseData->path   = pathStr;
    parseData->host   = NULL;

    //заголовки
    parseData->numHeaders = 0;
    const char *headersStart = lineEnd + 2; //  \r\n

    while (1) {
        const char *nextLineEnd = strstr(headersStart, "\r\n");
        if (!nextLineEnd || nextLineEnd > endOfHeaders) {
            break; // \r\n\r\n
        }
        if (nextLineEnd == headersStart) {
            break;
        }

        size_t headerLineLen = (size_t)(nextLineEnd - headersStart);
        char *headerLine = (char*)malloc(headerLineLen + 1);
        if (!headerLine) {
            free(requestLine);
            return -1;
        }
        memcpy(headerLine, headersStart, headerLineLen);
        headerLine[headerLineLen] = '\0';

        char *colonPos = strchr(headerLine, ':');
        if (!colonPos) {
            free(headerLine);
            free(requestLine);
            return -1;
        }

        *colonPos = '\0';
        char *headerName  = headerLine;
        char *headerValue = colonPos + 1;

        while (*headerValue == ' ' || *headerValue == '\t') {
            headerValue++;
        }

        httpHeader_t *hdr = &parseData->headers[parseData->numHeaders];
        hdr->name      = strdup(headerName);
        hdr->name_len  = strlen(headerName);
        hdr->value     = strdup(headerValue);
        hdr->value_len = strlen(headerValue);

        parseData->numHeaders++;

        // сохраняем заголовок Host
        {
            char lower[16];
            for (int i = 0; i < (int)hdr->name_len && i < 15; i++) {
                lower[i] = (char)tolower((unsigned char)hdr->name[i]);
                lower[i+1] = '\0';
            }
            if (strcmp(lower, "host") == 0) {
                parseData->host = hdr->value;
            }
        }

        free(headerLine);
        headersStart = nextLineEnd + 2;

        if (parseData->numHeaders >= 20) {
            break;
        }
    }

    size_t totalHeaderSize = (endOfHeaders - buf) + 4;
    return (ssize_t)totalHeaderSize;
}

ssize_t parse_response(const char *buf, size_t buflen, httpResponse *resp) {
    const char *endOfHeaders = strstr(buf, "\r\n\r\n");
    if (!endOfHeaders) {
        return 0;
    }

    // Парсим статус
    const char *lineEnd = strstr(buf, "\r\n");
    if (!lineEnd) return -1;
    size_t statusLineLen = (size_t)(lineEnd - buf);

    char *statusLine = malloc(statusLineLen + 1);
    if (!statusLine) return -1;
    memcpy(statusLine, buf, statusLineLen);
    statusLine[statusLineLen] = '\0';

    char *token = strtok(statusLine, " "); // HTTP/1.1
    token = strtok(NULL, " ");             // 200
    if (!token) {
        free(statusLine);
        return -1;
    }
    resp->status = atoi(token);
    free(statusLine);

    resp->numHeaders = 0;
    resp->contentLength = 0;
    const char *headersStart = lineEnd + 2;

    while (1) {
        const char *nextLineEnd = strstr(headersStart, "\r\n");
        if (!nextLineEnd || nextLineEnd > endOfHeaders) {
            break;
        }
        if (nextLineEnd == headersStart) {
            break;
        }

        size_t headerLineLen = (size_t)(nextLineEnd - headersStart);
        char *headerLine = malloc(headerLineLen + 1);
        if (!headerLine) return -1;
        memcpy(headerLine, headersStart, headerLineLen);
        headerLine[headerLineLen] = '\0';

        char *colonPos = strchr(headerLine, ':');
        if (!colonPos) {
            free(headerLine);
            return -1;
        }

        *colonPos = '\0';
        char *headerName  = headerLine;
        char *headerValue = colonPos + 1;

        while (*headerValue == ' ' || *headerValue == '\t') {
            headerValue++;
        }

        httpHeader_t *hdr = &resp->headers[resp->numHeaders];
        hdr->name      = strdup(headerName);
        hdr->name_len  = strlen(headerName);
        hdr->value     = strdup(headerValue);
        hdr->value_len = strlen(headerValue);

        resp->numHeaders++;

        // сохраняем Content-Length
        {
            char lower[32];
            for (int i = 0; i < (int)hdr->name_len && i < 31; i++) {
                lower[i] = (char)tolower((unsigned char)hdr->name[i]);
                lower[i+1] = '\0';
            }
            if (strcmp(lower, "content-length") == 0) {
                long long cl = atoll(hdr->value);
                if (cl > 0) {
                    resp->contentLength = (size_t)cl;
                }
            }
        }

        free(headerLine);
        headersStart = nextLineEnd + 2;
        if (resp->numHeaders >= 20) {
            break;
        }
    }

    size_t totalHeaderSize = (endOfHeaders - buf) + 4;
    return (ssize_t)totalHeaderSize;
}
