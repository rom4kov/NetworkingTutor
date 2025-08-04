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
        3,
        "",
        "Now that we have obtained a valid results structure we can finally start
to build the actual business logic of the server. We will use the socket() syscall
to create a socket by passing it the information we acquired from getaddrinfo.",
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
        3,
        "",
        "But before we use the socket() system call — the heart of how we create
network communication endpoints — it’s worth understanding where this API comes
from. While getaddrinfo() is part of the same API family, the socket() call
represents the core concept: a socket as a file descriptor for network I/O.
That concept originated in BSD Unix...@",
        1,
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
        3,
        "History of the socket API",
        "@The socket() syscall is part of the socket API, introduced in 1983 
with BSD Unix as a unified way to perform network communication. Before this,
network programming was platform-specific and fragmented.",
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
        3,
        "",
        "BSD’s popularity in universities and early internet development
(e.g., ARPANET) helped its socket API gain wide adoption. Its design also fit
neatly into the Unix philosophy: sockets behave like file descriptors — you
can read(), write(), and close() them just like regular files. This made network
communication easier to integrate into existing Unix tools.",
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
        3,
        "",
        'Alternatives like Sun’s TLI/XTI existed, but BSD sockets prevailed
in practice. Over time, the API was adopted by POSIX and even Windows, which
implements a variation called Winsock — essentially a wrapper around the BSD
socket API. This broad adoption is why Berkeley sockets became the de facto
standard for network programming.@',
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
        3,
        "Using the socket() syscall",
        "@But let's go back to building our server: The code you were assigned 
to write to implement the getaddrinfo function should look something like this:",
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
        3,
        "",
        '#define _POSIX_C_SOURCE 200112L@
#include <stdio.h>@
#include <string.h>@
#include <stdlib.h>@
#include <netdb.h>@
@
int main(void)@
{@
    struct addrinfo hints, *res;@
    int status;@
@
    memset(&hints, 0, sizeof(hints));@
    hints.ai_family = AF_UNSPEC;@
    hints.ai_socktype = SOCK_STREAM;@
    hints.ai_flags = AI_PASSIVE;@
@
    if ((status = getaddrinfo(NULL, "8080", &hints, &res)) != 0)@
    {@
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));@
        exit(1);@
    }@
@
    freeaddrinfo(res);@
}',
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
        3,
        "",
        "It's not a problem if you did it in a slightly different way, the main
thing is it works.",
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
        3,
        "",
        "The results structure (`res` in the code above) contains all the fields
we need to get a socket file descriptor with the socket() system call. The
prototype of the latter looks like this:",
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
        3,
        "",
        "#include <sys/types.h>@
#include <sys/socket.h>@
@
int socket(int domain, int type, int protocol);",
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
        3,
        "",
        "The socket() system call creates a socket and returns its file
descriptor. Here's a breakdown of the parameters it expects:@
@
• domain: The address family (e.g., AF_INET for IPv4, AF_INET6 for IPv6).@
• type: The socket type (SOCK_STREAM for TCP, SOCK_DGRAM for UDP).@
• protocol: when an address / protocol family supports several protocols the
desired one can be specified here. In our case only one is supported so we
can put a `0` here.@
@
If the call to the socket() syscall is successful the return value is the
file descriptor of the socket. Otherwise -1 is returned and the global variable
`errno` will specify what error occurred.",
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
        3,
        "",
        "Once we have the socket, we still need to bind it to an address,
listen for incoming connections, and finally accept them. We'll cover that in
the next sections after you implemented the call to socket().@",
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
        3,
        "Your task",
        "@Implement the socket syscall by doing the following:@
@
• pass it the correct fields from the res struct which was filled out by the
call to getaddrinfo that you implemented in the last section. You can easily
make them out by taking a quick look the man page of the addrinfo struct.@
• save the return value in a variable@
• handle possible errors by checking the return value and printing out the error
message with functions like fprintf and perror() like you did in the last
section.",
        12,
        0
    );
