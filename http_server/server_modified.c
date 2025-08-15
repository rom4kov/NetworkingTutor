#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void)
{
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int status, sockfd, new_fd, received, sent;
    int yes = 1;
    char *buf = calloc(3000, 1);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, "8090", &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
            -1)
        {
            fprintf(stderr, "socket error: %s\n", strerror(errno));
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
        {
            fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
            close(sockfd);
            continue;
        }

        if ((status = bind(sockfd, p->ai_addr, p->ai_addrlen)) != 0)
        {
            fprintf(stderr, "bind error: %s\n", strerror(errno));
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "Failed to bind socket\n");
        exit(EXIT_FAILURE);
    }

    if ((status = listen(sockfd, 10)) != 0)
    {
        fprintf(stderr, "listen error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    addr_size = sizeof(their_addr);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) <= 0)
    {
        fprintf(stderr, "accept error: %s, new_fd: %i\n", strerror(errno),
                new_fd);
        exit(EXIT_FAILURE);
    }

    received = recv(new_fd, buf, 3000, 0);

    if (received != -1 && strncmp(buf, "GET /", 5) == 0)
    {
        buf[received] = '\0';

        // printf("bytes received: %i\n", received);
        printf("%s\n", buf);

        const char *response = "HTTP/1.1 200 OK\r\n"
            "Content-Length: 64\r\n"
            "\n"
            "<!doctype html><html><body><h1>FREE "
            "PALESTINE</h1></body></html>"
            "\n";

        sent = send(new_fd, response, strlen(response), 0);
    }

    close(sockfd);
    free(buf);
    printf("%i\n", res->ai_family);
    freeaddrinfo(res);
}
