INSERT
    OR IGNORE INTO materials (
        course_id,
        section_id,
        content_title,
        content,
        order_num
    )
VALUES
    (
        1,
        2,
        "",
        'In order to make an HTTP server work the first thing we need to do is
to tell it what the address and port is on which it should listen for incoming
connection requests and data.',
        0
    );

INSERT
    OR IGNORE INTO materials (
        course_id,
        section_id,
        content_title,
        content,
        order_num
    )
VALUES
    (
        1,
        2,
        "",
        "There is an old and a new way of achieving this. The old one was to
manually fill out a struct called ''addrinfo'', putting in the desired address
and port number, the version of the IP protocol one wants to use and other
relevant information:",
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
        'struct addrinfo {@
  int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.@
  int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC@
  int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM@
  int              ai_protocol;  // use 0 for "any"@
  struct sockaddr *ai_addr;      // struct sockaddr_in or _in6@
  size_t           ai_addrlen;   // size of ai_addr in bytes@
  char            *ai_canonname; // full canonical hostname@
@
  struct addrinfo *ai_next;      // linked list, next node@
};@
@
// `struct sockaddr` holds socket address information for many types of sockets.@
@
struct sockaddr {@
  unsigned short    sa_family;    // address family, AF_xxx@
  char              sa_data[14];  // 14 bytes of protocol@
};@',
        2,
        1
    );
