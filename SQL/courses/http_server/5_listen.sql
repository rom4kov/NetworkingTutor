INSERT
    OR IGNORE INTO materials (
        course_id,
        section_id,
        content_title,
        content,
        order_num,
        syntax_highlighting
    )
VALUES
    (
        1,
        5,
        "",
        "After having the system calls socket and bind implemented to get hold
of a socket file descriptor and to attach it to an address and port on our host
system, the next step is to 'activate' the socket and make it listen for incoming
connections. Let's first take a the code we've written so far.",
        0,
        0
    );

INSERT
    OR IGNORE INTO materials (
        course_id,
        section_id,
        content_title,
        content,
        order_num,
        syntax_highlighting
    )
VALUES
    (
        1,
        5,
        "",
        '#define _POSIX_C_SOURCE 200112L@
#include <errno.h>@
#include <netdb.h>@
#include <stdio.h>@
#include <stdlib.h>@
#include <string.h>@
#include <sys/socket.h>@
#include <unistd.h>@
@
int main(void)@
{@
    struct addrinfo hints, *res, *p;@
    int status, sockfd;@
    int yes = 1;@
@
    memset(&hints, 0, sizeof(hints));@
    hints.ai_family = AF_UNSPEC;@
    hints.ai_socktype = SOCK_STREAM;@
    hints.ai_flags = AI_PASSIVE;@
@
    if ((status = getaddrinfo(NULL, "8080", &hints, &res)) != 0)@
    {@
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));@
        exit(EXIT_FAILURE);@
    }@
@
    for (p = res; p != NULL; p = p->ai_next)@
    {@
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)@
        {@
            fprintf(stderr, "socket error: %s\n", strerror(errno));@
            continue;@
        }@
@
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))@
        {@
            fprintf(stderr, "setsockopt error: %s\n", strerror(errno));@
            close(sockfd);@
            continue;@
        }@
@
        if ((status = bind(sockfd, p->ai_addr, p->ai_addrlen)) != 0)@
        {@
            fprintf(stderr, "bind error: %s\n", strerror(errno));@
            close(sockfd);@
            continue;@
        }@
@
        break;@
    }@
@
    if (p == NULL)@
    {@
        fprintf(stderr, "Failed to bind socket\n");@
        exit(EXIT_FAILURE);@
    }@
@
    freeaddrinfo(res);@
    close(sockfd);@
}@',
        1,
        1
    );

INSERT
    OR IGNORE INTO materials (
        course_id,
        section_id,
        content_title,
        content,
        order_num,
        syntax_highlighting
    )
VALUES
    (
        1,
        5,
        "",
        'The listen() system call that we are going to implement now does the
following: it "marks" the socket we already have as a special kind of socket,
one that has the purpose of accepting incoming connection requests. This is
exactly what we want for out server.',
        0,
        0
    );

INSERT
    OR IGNORE INTO materials (
        course_id,
        section_id,
        content_title,
        content,
        order_num,
        syntax_highlighting
    )
VALUES
    (
        1,
        5,
        "",
        'Another thing the listen() syscall does is putting in place a queue
of connection requests so that they can be handled one after another. The
size of that queue, i.e. the amount of requests that can wait inside it,
can be specified via the backlog argument of the listen() syscall. Its
prototype looks like this:',
        0,
        0
    );

INSERT
    OR IGNORE INTO materials (
        course_id,
        section_id,
        content_title,
        content,
        order_num,
        syntax_highlighting
    )
VALUES
    (
        1,
        5,
        "",
        '@#include <sys/socket.h>@
@
int listen(int sockfd, int backlog);@',
        0,
        0
    );
