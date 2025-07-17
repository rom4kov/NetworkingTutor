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
        "// From the man page for `sockaddr_in`:@
@
#include <netinet/in.h>@
@
struct sockaddr_in {@
  sa_family_t     sin_family;     /* AF_INET */@
  in_port_t       sin_port;       /* Port number */@
  struct in_addr  sin_addr;       /* IPv4 address */@
};@
@
struct in_addr {@
    uint32_t s_addr;@
};",
        2,
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
        2,
        "",
        "This `sockaddr_in` stuct in turn was invented as a variation to
an older struct called `sockaddr`, to provide a better way to deal specifically
with internet domain sockets that use IPv4 (therefore the '_in' in its name.):",
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
        2,
        "",
        "// Same man page as `sockaddr_in`:@
@
#include <sys/socket.h>@
@
struct sockaddr {@
  sa_family_t     sa_family;      /* Address family */@
  char            sa_data[];      /* Socket address */@
};",
        4,
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
        2,
        "",
        "These two structs can be type-cast to each other without problem.
And this is actually important to remember, since you will have to do exactly
this later, when you pass the information to structs like bind() and connect().@
@
You used the struct like this (and one can still see this in a lot of older and 
even not-so-old socket code):",
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
        2,
        "",
        "struct sockaddr_in addr;@
addr.sin_family = AF_INET;@
addr.sin_port = htons(8080);@
addr.sin_addr.s_addr = INADDR_ANY;",
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
        2,
        "",
        "Then later you would pass the struct to the bind() or some other syscall.@
@
But what are the all these fields inside the struct and what values do you assign to
them? Let's go through them one by one.",
        7,
        0
    );
