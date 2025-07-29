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
        "Now that we have obtained a valid results structure we can start
to build the actual business logic of the server. The code you were assigned
to write to implement the getaddrinfo function should look something like this:",
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
        1,
        1
    );


