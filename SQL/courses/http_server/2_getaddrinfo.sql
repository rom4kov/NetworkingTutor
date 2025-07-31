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
portable way using getaddrinfo().@",
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
sockaddr* when using functions like bind().@",
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
        '@• sin_family = AF_INET specifies the IPv4 protocol (Layer 3).@
• sin_port = htons(8080) sets the port to 8080, converting it to network byte
order (big endian), which is required by the kernel.@
• sin_addr.s_addr = INADDR_ANY means "listen on all available interfaces"
(i.e., 0.0.0.0).@
@
To connect to a specific IP instead, you’d use inet_pton() to convert an IP
address string to binary format.@',
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
        '@Manually configuring address structs works, but it’s limited:@
• It only supports IPv4.@
• It can’t resolve domain names (no DNS support).@
• It’s less portable across systems.@
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
        "This function replaces older functions like gethostbyname() and
provides a unified interface for both IPv4 and IPv6.",
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
        "struct addrinfo hints, *res;@
@
memset(&hints, 0, sizeof(hints));@
@
hints.ai_family = AF_UNSPEC;@
hints.ai_socktype = SOCK_STREAM;@
hints.ai_flags = AI_PASSIVE;",
        14,
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
        "Here we choose AF_UNSPEC for the ai_family field, which tells
getaddrinfo to return a address for any address family (either IPv4 or IPv6).
But you could also put AF_INET for IPv4 or AF_INET6 for IPv6 if getting a
specific address family is important to you intructions." ,
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
        "For the socket type we pick SOCK_STREAM which gives us a TCP socket,
providing a stable and reliable connection (as discussed in the previous section
of this course). The value AI_PASSIVE for the field `ai_flags` will (if the `node`
parameter of getaddrinfo is set to NULL) return a 'wildcard address' like 
INADDR_ANY did in the 'old way' that is suitable to use with later syscalls like
bind() and and accept() that are needed to build a server." ,
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
        "To use getaddrinfo we have to pass it four arguments:@
@
• the Internet host (`node`) - an IP address or a domain name:
setting NULL here with AI_PASSIVE in hints will get your a
wildcard address, which is desired when building a server.@
• the port or service you want to use@
• some 'hints' in the form of a struct `addrinfo`@
• another struct addrinfo that will be filled out with the
results of the function call (`res` in the prototype)@
@",
        17,
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
        "Your task",
        "@Now it's your turn to implement the call to getaddrinfo. 
To complete the test for this section, your implementation must:@
@
• include the required header files@
• Declare and set up the hints struct, including the three fields we discussed above@
• Pass the correct parameters to getaddrinfo@
• Store the return value and check for errors@
• If an error occurs, use fprintf to print a descriptive message using
gai_strerror() to stderr@
• Copy the resulting addrinfo struct into a newly malloc'ed one (i.e., don't
return the original directly)@
• Free the original result using freeaddrinfo()@
• Return your copy of the result@
@
Hint: If you get stuck or you're not sure about something, take a look at the
example in the getaddrinfo man page (man getaddrinfo). It includes a complete
call and may help clarify how the pieces fit together." ,
        18,
        0
    );
