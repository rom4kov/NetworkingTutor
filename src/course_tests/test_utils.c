#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include "../models/models.h"
#include "pcre2.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int check_line_for_matches(LINE *current_line, int j, pcre2_code **re,
                           size_t subject_length)
{
    PCRE2_SPTR subject = (PCRE2_SPTR)current_line->buf_;
    pcre2_match_data *md = pcre2_match_data_create_from_pattern(re[j], NULL);
    if (!md)
        return -1;

    return pcre2_match(re[j], subject, subject_length, 0, 0, md, NULL);
}

void *connect_to_server(void *arg)
{
    int *port = (int *)arg;
    napms(500);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(*port);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        return NULL;
    }

    for (int attempt = 0; attempt < 30; attempt++)
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            perror("socket");
            return NULL;
        }

        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0)
        {
            close(sock);
            return NULL;
        }

        close(sock);
        napms(100);
    }
    fprintf(stderr, "connect timeout, port: %i\n", *port);

    return NULL;
}

void *connect_to_server_and_check_response(void *arg)
{
    int *port = (int *)arg;
    char buf[512];
    bool *response_ok = malloc(sizeof(bool));

    napms(500);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(*port);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        return NULL;
    }

    for (int attempt = 0; attempt < 30; attempt++)
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            perror("socket");
            return NULL;
        }

        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0)
        {
            send(sock, "GET / HTTP/1.1\r\n", 16, 0);
            recv(sock, buf, 512, 0);
            fprintf(stdout, "Response: %s", buf);
            if (strncmp(buf, "HTTP/1.1", 8) == 0)
            {
                *response_ok = true;
                close(sock);
                return (void *)response_ok;
            }
        }

        close(sock);
        napms(100);
    }
    fprintf(stderr, "connect timeout, port: %i\n", *port);

    *response_ok = false;
    return (void *)response_ok;
}
