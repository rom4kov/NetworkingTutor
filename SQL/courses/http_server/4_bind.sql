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
        4,
        "",
        "Great — now that we have a valid socket file descriptor, we’re ready
to move forward. Your server code should currently look something like this
(and again, no worries if your version isn’t exactly identical):",
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
        4,
        "",
        '@#define _POSIX_C_SOURCE 200112L@
#include <stdio.h>@
#include <string.h>@
#include <stdlib.h>@
#include <netdb.h>@
#include <errno.h>@
@
int main(void)@
{@
    struct addrinfo hints, *res;@
    int status, sockfd;@
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
    if ((sockfd = socket(res->ai_family, res->ai_socktype,@ res->ai_protocol)) == -1)@
    {@
        fprintf(stderr, "socket error: %s\n", strerror(errno));@
    }@
@
    freeaddrinfo(res);@
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
        4,
        "Why bind()?",
        "@The socket descriptor returned by socket() is just a file descriptor
— it doesn’t yet represent a real, usable network endpoint. To make it usable
(i.e., to allow the OS to associate it with a specific port and IP address),
we need to call bind().",
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
        4,
        "",
        "This is almost always required on the server side, because you want
your server to listen on a known, fixed port that clients can connect to. On
the client side, calling bind() is typically unnecessary — the OS will
automatically choose an available local port when you call connect().@",
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
        4,
        "Historical sketch of the bind() syscall",
        "@The bind() system call was introduced with the BSD sockets API in
4.2BSD (1983). Its design reflects the Unix philosophy: just as open()
associates a file descriptor with a file path, bind() associates a socket
with a specific IP address and port.",
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
        4,
        "",
        "This was essential to writing server daemons — programs that listen
on well-known ports for incoming requests. By giving developers direct control
over IP/port binding, bind() became a central piece of network programming on
Unix and beyond.@",
        5,
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
        4,
        "Implementing bind()",
        "@The call to bind is actually fairly easy to implement. This is what
its prototype looks like:",
        6,
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
        4,
        "",
        "@#include <sys/types.h>@
#include <sys/socket.h>@
@
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);@",
        7,
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
        4,
        "",
        "The parameters for bind() are almost self-explanatory:@
@
• sockfd: the socket file descriptor returned by socket()@
• addr: the address structure (e.g., from res->ai_addr) provided by getaddrinfo()@
• addrlen: the length of the address (e.g., res->ai_addrlen)@
@
Try implementing the bind() call on your own using these fields. Don’t forget
to include error handling with perror() or strerror() as you did with socket().@",
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
        4,
        "Use setsockopt() to allow reuse of local addresses",
        '@Sometimes bind() fails with an "Address already in use" error.
This can happen if the port was recently used by another process or hasn''t yet been released by the kernel.
@
To fix this, add the following after your socket() call and before bind():@',
        9,
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
        4,
        "",
        'if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)@
{@
    perror("setsockopt");@
    exit(EXIT_FAILURE);@
}@',
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
        4,
        "",
        "This tells the OS it’s okay to reuse the port, even if it's in a
TIME_WAIT state.@",
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
        4,
        "Picking the Right Address: Iterating Over getaddrinfo() Results",
        "@There's one feature of getaddrinfo we haven't taken advantage of yet.
getaddrinfo() doesn’t return just one address — it returns a linked list of
address candidates. Some of them may not work (e.g., unsupported address family
, already-used port, etc.), so the common approach is to loop through the
list until one succeeds.",
        11,
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
        4,
        "",
        "The standard pattern is to use a for loop where you iterate through
the linked list in of addresses in the addrinfo struct by putting something
like `addr = addr->ai_next` in the increment expression of the for loop.
Inside the for loop you call socket(), setsockopt() and bind() each with the
values of the current node in the linked list.@",
        12,
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
        4,
        "You task",
        "@Do the following to complete your implementation of bind and integrate it
into the code you've written so far:@
@
• Implement bind() using the correct fields from the addrinfo struct@
• Add setsockopt() to handle address reuse@
• Wrap socket(), setsockopt(), and bind() in a loop over the addrinfo linked list@
• Add proper error handling for all system calls",
        13,
        0
    );
