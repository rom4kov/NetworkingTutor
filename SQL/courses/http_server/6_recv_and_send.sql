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
        6,
        "",
        "If you're reading this it means that you successfully completed the
last section on the listen() and accept() system calls. Congrats! We’re now
very close to the end of this course. There are only a few more steps before
we have a working minimal HTTP server.",
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
        6,
        "",
        "Let’s start by looking at the code you should have from the previous
section, added to your existing server.c file:",
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
        6,
        "",
        '@if ((status = listen(sockfd, 10)) != 0)@
{@
    fprintf(stderr, "listen error: %s\n", strerror(errno));@
    exit(EXIT_FAILURE);@
}@
@
addr_size = sizeof(their_addr);@
if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) <= 0)@
{@
    fprintf(stderr, "accept error: %s, new_fd: %i\n", strerror(errno), new_fd);@
    exit(EXIT_FAILURE);@
}@
@
freeaddrinfo(res);@
close(sockfd);@',
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
        6,
        "",
        "At this point, your server is listening for incoming connection requests.@
Whenever a request comes in, accept() creates a new socket specifically for that
connection. It also fills in the sockaddr_storage struct (the second argument)
with the address information of the client.@",
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
        6,
        "Receiving data with recv()",
        "@Now we can read the data sent by the client over the new socket 
returned by accept(). We use the recv() function from the socket API for this.
In its default blocking mode, recv() will check whether the socket has any
incoming data, and if not, it will wait until data is available before returning.",
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
        6,
        "",
        "This waiting happens because the kernel puts the calling process to
sleep and wakes it up only when new data arrives for that socket, avoiding
busy-waiting and unnecessary CPU usage. The function looks like this:",
        5,
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
        6,
        "",
        "@#include <sys/socket.h>@
@
ssize_t recv(int socket, void *buffer, size_t length, int flags);@",
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
        6,
        "",
        "The parameters are:@
@
• socket: the socket file descriptor returned by accept()@
• buffer: a pointer to memory where the received data will be stored@
• length: the size of that buffer in bytes@
• flags: special options (we don't need any here, so pass 0)",
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
        6,
        "",
        "For our purposes, a buffer of 512 to 4096 bytes is typical. In this
project we only need to read the first line of the request — the HTTP request
line — so 512 bytes is plenty. You can also make it 1024, if you'd like to print
out and see the whole HTTP request message from the client in the terminal.",
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
        6,
        "",
        "Because the buffer is only used inside this function, we can declare
it on the stack rather than allocating it dynamically with malloc():",
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
        6,
        "",
        "@char buf[512];@",
        10,
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
        6,
        "",
        "Call recv() and store the return value in a variable. This return
value is the number of bytes received, or -1 if there was an error.@
We’ll only send a response if recv() succeeds and the message is a valid HTTP
request.@",
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
        6,
        "Sending data with send()",
        '@The last step to get a minimal HTTP server running is to send a 
response back to the client.@
@
First, check that:@
@
1. recv() did not return -1@
2. The first bytes of the request match "GET /" (use strncmp() for this)@
@
If both checks pass, use the send() function:',
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
        6,
        "",
        "@#include <sys/socket.h>@
@
ssize_t send(int sockfd, const void *buf, size_t size, int flags);@",
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
        6,
        "",
        "Pass it the following arguments:@
@
• sockfd: socket file descriptor returned from accept()@
• buf: pointer to the data you want to send (our HTTP response)@
• size: the number of bytes to send@
• flags: 0 for our case@",
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
        6,
        "A minimal HTTP response",
        "@If the request is valid, we can respond with the string:@
@
HTTP/1.1 200 OK\n\n@
@
• HTTP/1.1: tells the client we are speaking HTTP, version 1.1@
• 200 OK: status code meaning “success”@
• The two \n characters mark the end of the HTTP headers@
@
This alone is enough for the browser to accept the connection, though it will
just display a blank page.@",
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
        6,
        "Note: Spec vs. Reality - HTTP Line Endings",
        "@The HTTP/1.1 specification (RFC 7230) requires that every line in 
a response header, including the status line and the final blank line, be
terminated with a carriage return + line feed (\r\n). For example:@
@
HTTP/1.1 200 OK\r\nContent-Length: 64\r\n\r\n.",
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
        6,
        "",
        "Modern browsers, however, are forgiving and will often accept a lone
line feed (\n) instead, a tolerance inherited from early HTTP servers on Unix
systems. In this course, we use \n for simplicity so you can get results
quickly, but in production you should follow the spec and use \r\n to ensure
full compatibility with all HTTP clients and intermediaries.@",
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
        6,
        "Adding HTML content",
        "@Let’s make it more interesting by adding some HTML:@
@
<!doctype html><html><body><h1>Hello, world!</h1></body></html>",
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
        6,
        "",
        "HTTP responses that include a body should also include a Content-Length
header so the client knows how many bytes to expect.@
Our HTML here is 63 bytes, so the complete HTTP response string is:@
@
HTTP/1.1 200 OK\n@
Content-Length: 63\n@
\n@
<!doctype html><html><body><h1>Hello, world!</h1></body></html>@
@
Pass this string to send() as the buf argument, and specify its size as the
size argument.@",
        19,
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
        6,
        "Testing",
        "@Compile and run your server, then in your browser go to:@
@
http://localhost:<port_you_set>@
@
If everything is correct, you should see “Hello, world!” in the browser.@
For extra confirmation, open your browser’s Developer Tools (F12), go to the
Network tab, reload the page, and click the localhost entry — you should see
a green 200 OK status.@",
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
        6,
        "Your task",
        "@
• Implement recv() and send()@
• Check the received data for a valid HTTP GET request@
• Send a proper HTTP response containing an HTML <h1>Hello, world!</h1>",
        21,
        0
    );
