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
        "The field `sin_family` which is of the type `sa_family_t` specifies
the (socket) address family. In our case you put AF_INET here, which stands for
Internet Protocol version 4 (IPv4). As you surely remember ;), IP is the main
protocol of layer 3, the network layer, as we learned in the last section.",
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
        2,
        "",
        "Next comes the field `sin_port` of type `in_port_t`. It allows us
to define which port we want our server to listen on. But there's a tricky
part about this: we can't just put a plain old integer here. Since network
addresses and port numbers are dealt with in binary form in the kernel there
can be two forms of notation, big endian and little endian.",
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
        2,
        "",
  "And because the network stack uses the big endian notation, but some operating
systems use little endian we have to convert the from one to the other when setting
the value of this field. There are special functions that do this for us: ntohs()
converts from network byte order to host byte order and htons() vice versa (h to n,
the s stands for short type).",
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
        2,
        "",
        "Finally, the last field is another struct: `in_addr`, that contains the
actual address we want to listen on (or connect to if you would build a client instead
of a server) in its 4-byte (uint32_t type) big field `s_addr`.",
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
        2,
        "",
        "You usually set this field to the constant INADDR_ANY which is just
a macro for the IP address 0.0.0.0 which tells the kernel to bind the socket
to all IP addresses that are available on the machine so that it will listen to
incoming requests on all network interfaces of the system.",
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
        2,
        "",
        "If you were writing client code where the aim is to connect to a
specific address you also would have to convert the IP address to binary form
using a function like inet_pton().
",
        13,
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
        "As you can already see, all this manual configuration amounts to quite
effort. But apart from that it also has a couple of other drawbacks, especially
hen a server application becomes more complex and has to scale to meet more
challenging demands:",
        14,
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
        "• it's not very portable@
• it can only handle IPv4, not IPv6@
• it's unable to do DNS resolution@
• you can't pass service names like 'http' or 'sftp' to it",
        15,
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
        "Because of all of these shortcomings network architects came
together and invented the new syscall `getaddrinfo()` that provides us
network programmers with a much more handy way to obtain the address
information we need. It looks like this:",
        16,
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
        "#include <sys/types.h>@
#include <sys/socket.h>@
#include <netdb.h>@
@
int getaddrinfo(const char *restrict node,@
                const char *restrict service,@
                const struct addrinfo *restrict hints,@
                struct addrinfo **restrict res);@
@
void freeaddrinfo(struct addrinfo *res);@
@
const char *gai_strerror(int errcode);@",
        17,
        1
    );
