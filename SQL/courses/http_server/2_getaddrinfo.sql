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
        2,
        "",
        'In order to make an HTTP server work the first thing we need to do is
to tell it what the address and port is on which it should listen for incoming
connection requests and data.',
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
        2,
        "",
        "There is an old and a new way of achieving this. The old one was to
manually fill out a struct called 'sockaddr_in', putting in the desired address
and port number, the version of the IP protocol one wants to use and other
relevant information:",
        1,
        0,
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
        2,
        "",
        "// From the man page for `sockaddr_in`:@
@
#include <netinet/in.h>@
@
struct sockaddr_in {@
  sa_family_t     sin_family;     /* AF_INET */@
  in_port_t       sin_port;       /* Port number */@
  struct in_addr  sin_addr;       /* IPv4 address */@
};@",
        2,
        1
    );
