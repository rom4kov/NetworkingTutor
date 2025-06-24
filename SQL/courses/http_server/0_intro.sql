CREATE TABLE IF NOT EXISTS materials (
    id INTEGER PRIMARY KEY,
    course_id INTEGER NOT NULL,
    section_id INTEGER NOT NULL,
    content_title TEXT,
    content TEXT,
    order_num INTEGER,
    FOREIGN KEY(course_id) REFERENCES courses(id) UNIQUE(course_id, section_id, order_num)
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
        0,
        "",
        '  _     _   _             ____@
          | |__ | |_| |_ _ __ _   / / /@
          | `_ \| __| __| `_ (_) / / / @
          | | | | |_| |_| |_) | / / /  @
          |_| |_|\__|\__| .__(_)_/_/   @
                        |_|            @',
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
        0,
        "",
        'So why of all things build an HTTP server as the first project
to learn network programming? There are many good reasons for doing so,
but here we want to emphasize three in particular:',
        1
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
        0,
        "1. It demystifies how the web works.",
        "By building an HTTP server from scratch, you'll finally see
what really happens when a browser makes a request. No black boxes, just
real sockets, headers, and responses.",
        2
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
        0,
        "2. It’s the perfect starting point for network programming.",
        "You don’t need any prior knowledge of the network stack to begin,
and you'll walk away with a concrete understanding of core concepts like
sockets, ports, and protocols.",
        3
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
        0,
        "3. It lays a solid foundation for any future path.",
        "Whether you plan to work with high-level web frameworks or dive
into low-level server architecture, this project teaches you the essential
building blocks that apply to both.",
        4
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
        0,
        "What to expect",
        "The course is divided into short, focused sections. Each one introduces
a key system call, concept, or component involved in building a working HTTP server in C.",
        5
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
        0,
        "",
        "1. Basics: TCP/IP + HTTP   2. getaddrinfo()   3. socket()   
4. bind()   5. listen()   6. accecpt()   7. recv()   8. send()
9. send an HTTP headers with HTML file",
        6
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
        0,
        "",
        "At the end of each section, you’ll implement what you’ve learned by
coding a small part of the server. Once your solution passes the tests,
you’ll earn progress points and unlock the next step.",
        7
    );
