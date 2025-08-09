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
    int status, sockfd, new_fd;
    int yes = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, "8080", &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
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
        fprintf(stderr, "listen error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    addr_size = sizeof(their_addr);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) == 0)
    {
        fprintf(stderr, "listen error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);
    close(sockfd);
}
