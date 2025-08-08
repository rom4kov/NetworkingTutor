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
        'After calling socket() and bind() to create a socket and attach it to
an address and port on our host system, the next step is to "activate" the
socket so it can listen for incoming connections. Let’s first look at the code
we’ve written so far:',
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
        '@#define _POSIX_C_SOURCE 200112L@
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
        "What listen() does",
        '@The listen() system call marks the socket as a passive socket — one 
that is used to accept incoming connection requests.',
        2,
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
        'This call doesn’t itself block. It simply tells the OS: “This socket
should now be ready to receive connections.',
        3,
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
        'It also sets up a queue (called the backlog) of incoming connections.
The backlog size is specified as the second argument. The prototype of the
function is:',
        4,
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
        6,
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
        "Let's use it with the socket file descriptor we already obtained from
previous syscalls as the first argument and a value of maybe 10 or 20 as the
second for the backlog.@",
        7,
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
        "What accept() does",
        "@Once the socket is listening, we can call accept() to accept one of
the pending connections from the queue.@
@
The accept() function does block until a connection attempt is made by a client.
Its prototype looks like this:",
        8,
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
        "@#include <sys/socket.h>@
@
int accept(int sockfd, struct sockaddr *_Nullable restrict addr,@
           socklen_t *_Nullable restrict addrlen);@",
        9,
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
        "The accept() system call takes three arguments:@
@
• sockfd: the socket file descriptor from socket()@
• addr: a pointer to a struct sockaddr that will be filled in with the client’s
address the connection request.@
• addrlen: a pointer to the size of that structure@
@
Since we don’t know in advance whether the client will use IPv4 or IPv6, we
declare the address with type sockaddr_storage, which is large enough for
either. When passing it to accept(), we cast it to sockaddr *.@",
        10,
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
        "Your task",
        "@Complete the following assignments to finish this section:@
@
• Implement both the listen() and the accept() system calls.@
• Make sure both calls include proper error handling (check return values and
use perror() or fprintf() with strerror() as needed)@
• Declare the client address variable as a sockaddr_storage, and cast it to
sockaddr* when calling accept()@
  ",
        12,
        0
    );
