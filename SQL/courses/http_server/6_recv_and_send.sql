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
last section on the listen() and accept() system calls. Congrats! Now we're
reaching the end of this course. There's only a few things left to finish
building our simple HTTP server.",
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
        "Let's first look at the code you ideally wrote in the last section,
adding it to the already existing code in the server.c file:",
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
        "We're not showing the whole server.c file again, the code up the new
part added in the last section should have remained unchanged.",
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
        "",
        "So what do we need to do now? The server is listening for incoming 
connection requests and any time a request comes in, accept() will create a
brand new socket specifically for that new connection. accept() will also fill
out the sockaddr_storage struct you passed it as the second argument with the
address information of the client that connected to your server.",
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
        "",
        "With all that we can now tell the server to read any message data that
might have arrived from a remote machine at the new socket that was created by
the accept() system call. We use the recv() function for that purpose. recv()
will ask the socket if has any data and block (wait) if that's not the case. It
is also provided by the socket API:",
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
        "@#include <sys/socket.h>@
@
ssize_t recv(int socket, void *buffer, size_t length, int flags);@",
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
        "You have to pass four arguments to recv()@:
@
• socket: the socket file descriptor returned by accept()@
• buffer: a pointer to a buffer to hold that data from the socket@
• length: the size of that buffer 
• flags: additional options like if the data should include out-of-band
messages We don't need any of that here and can pass 0.",
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
        6,
        "",
        "Declare the buffer before calling recv() and give it at reasonable size.
A standard size would be 4096 bytes, but we can go with less for this very
simple server. At the end we just need to read the first line of the request
containing the HTTP request, not all the HTTP headers which come after.",
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
        6,
        "",
        "We don't need to allocate the buffer on the heap with malloc, we can
just create a simple array on the stack since we don't need to use it outside
of the function it is initialized at.",
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
        "When you've done that, save the return value of recv() in a variable
that we can check afterwards. We will send a response to the client that connected
to our server only if the call to recv() didn't fail and if the message sent is
a valid HTTP request",
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
        "Using send()",
        "And that's the last thing to do to practically have a minimal working
HTTP server: check if the return value of recv() is not -1 and if that's true
use a function like strncmp() to check if the first 5 bytes of the request
message that should now be in the buffer are equal to 'GET /'.",
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
        "If it is we can leverage the send() function from the socket API to
send a rudimentary HTTP response back to the client:",
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
        6,
        "",
        "@#include <sys/socket.h>@
@
ssize_t send(int sockfd, const void *buf, size_t size, int flags);@",
        11,
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
• sockfd: socket file descriptor returned by the call to accept()@
• buf: a string with the HTTP response@
• size: the size of this string@
• flags: 0 again, since we don't need any options here as well@
@
Let the `buf` argument be a string with a valid HTTP response:",
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
        "In this case where the request from the client was a legitimate HTTP
request (which we know because we checked) a string with the appropriate
HTTP response looks like this:@
@
HTTP/1.1 200 OK\n\n@",
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
        "We send 'HTTP/1.1' to tell the client that we're 'talking' HTTP and
the version to inform them which 'dialect' of HTTP it is, so to speak. The
`200 OK` is a standard HTTP code telling the client there were no problems with
its request and that the requested will be provided below.",
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
        6,
        "",
        "And that's it! You can compile the code, run the executable, go to your
browser and type `localhost:<the port you specified in server.>` and hit enter.
If the browser does not show an sad smiley with an error message saying something
like 'This page isn’t working.' but just an empty screen, it means that our
server is working.",
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
        "",
        "To verify you can run the server executable again open the Developer
Tools of your browser with F12, go to the network tab and reload the page. Then,
if everything worked, you should see an entry in the table saying `localhost`.
If you click on it there should be a line with a green dot showing the status
code to be `200 OK`",
        14,
        0
    );
