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
        "Before an HTTP server can listen for incoming connections, it needs
to know on which address and port to listen. There are two main approaches for
specifying this information in C: the old manual way and the newer, more
portable way using getaddrinfo().",
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
        "The Old Way: Manually Filling a sockaddr_in Struct",
        "There is an old and a new way of achieving this. Historically,
programmers manually filled a sockaddr_in struct like this:",
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
        "#include <netinet/in.h>@
@
struct sockaddr_in addr;@
addr.sin_family = AF_INET;@
addr.sin_port = htons(8080);@
addr.sin_addr.s_addr = INADDR_ANY;",
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
        "This approach uses the sockaddr_in struct:",
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
        "// From the man page for `sockaddr_in`:@
@
struct sockaddr_in {@
    sa_family_t     sin_family;     /* AF_INET */@
    in_port_t       sin_port;       /* Port number */@
    struct in_addr  sin_addr;       /* IPv4 address */@
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
        "The sin_addr field is itself a struct:",
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
        "struct in_addr {@
    uint32_t s_addr;@
};",
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
        "This was introduced as an IPv4-specific alternative to the more generic:",
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
        "// Same man page as `sockaddr_in`:@
@
#include <sys/socket.h>@
@
struct sockaddr {@
  sa_family_t     sa_family;      /* Address family */@
  char            sa_data[];      /* Socket address */@
};",
        8,
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
        "Both types are compatible, and you’ll often cast a sockaddr_in* to
sockaddr* when using functions like bind().",
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
        'Explanation of the Fields',
        '• sin_family = AF_INET specifies the IPv4 protocol (Layer 3).@
@
• sin_port = htons(8080) sets the port to 8080, converting it to network byte
order (big endian), which is required by the kernel.@
@
• sin_addr.s_addr = INADDR_ANY means "listen on all available interfaces"
(i.e., 0.0.0.0).@
@
To connect to a specific IP instead, you’d use inet_pton() to convert an IP
address string to binary format.',
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
        'The Modern Way: Using getaddrinfo()',
        'Manually configuring address structs works, but it’s limited:@
@
• It only supports IPv4.@
@
• It can’t resolve domain names (no DNS support).@
@
• It’s less portable across systems.@
@
• You can’t use service names like "http".@
@
To address these issues, POSIX introduced getaddrinfo():',
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
const char *gai_strerror(int errcode);",
        12,
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
        "struct addrinfo hints, *res;@
@
memset(&hints, 0, sizeof(hints));@
@
hints.ai_family = AF_UNSPEC;@
hints.ai_socktype = SOCK_STREAM;@
hints.ai_flags = AI_PASSIVE;",
        13,
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
        "Next comes the field `sin_port` of type `in_port_t`. It allows us to 
define which port we want our server to listen on, so we're now on layer 4. But
there's a tricky part about this: we can't just put a plain old integer here.
Since network addresses and port numbers are dealt with in binary form in the
kernel there can be two forms of notation, big endian and little endian.",
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
        "The modern way: getaddrinfo()",
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
        "To use it you have to pass it four arguments:@
@
• the Internet host (`node`) - an IP address or a domain name@
• the port or service you want to use@
• some 'hints' in the form of a struct `addrinfo`@
• another struct addrinfo that will be filled out with the@
  results of the function call (`res` in the prototype)@
@
This means you first fill out the hints addrinfo struct like this (after
zeroing it out useing memset):",
        18,
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
        "Here we choose AF_UNSPEC for the ai_family field, which tells
getaddrinfo to return a address for any address family (either IPv4 or IPv6).
But you could also put AF_INET for IPv4 or AF_INET6 for IPv6 if getting a
specific address family is important to you intructions." ,
        20,
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
        "For the socket type we pick SOCK_STREAM which gives us a TCP socket,
providing a stable and reliable connection (as discussed in the previous section
of this course). The value AI_PASSIVE for the field `ai_flags` will (if the `node`
parameter of getaddrinfo is set to NULL) return a 'wildcard address' like 
INADDR_ANY did in the 'old way' that is suitable to use with later syscalls like
bind() and and accept() that are needed to build a server." ,
        21,
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
        "The actual implementation of the call to getaddrinfo is your task now.
To complete this sections's test you have to not only pass it the right parameters
but also:@
@
• include the necessary header files@
• declare the hints struct and initialize the three fields we discussed above@
• handle possible errors by saving the return value in a variable@
• print the error message with the gai_strerror function to stderr@
• copy the results struct into a new one that you allocated memory for@
• free the results addrinfo struct with freeaddrinfo@
• return the copy of the results struct" ,
        22,
        0
    );
