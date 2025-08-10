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
building out simple HTTP server.",
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
brand new socket specifically for that new connection and fill out the
sockaddr_storage stuct you passed it as the second argument with the address
information of the client that connected to your server.",
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
brand new socket specifically for that new connection and fill out the
sockaddr_storage stuct you passed it as the second argument with the address
information of the client that connected to your server.",
        4,
        0
    );
