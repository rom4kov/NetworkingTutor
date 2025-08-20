CREATE TABLE IF NOT EXISTS courses (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    short_desc TEXT,
    ascii_logo TEXT,
    end_of_course_msg TEXT
);

INSERT
    OR IGNORE INTO courses (
        name,
        short_desc,
        ascii_logo,
        end_of_course_msg
    )
VALUES
    (
        'Build an HTTP Server in C',
        'In this first and short project you will learn how to build a simple
HTTP server in C, using the very basic version 1.1 of the HTTP protocol.',
        ' _     _   _             ____@
| |__ | |_| |_ _ __ _   / / /@
| `_ \| __| __| `_ (_) / / / @
| | | | |_| |_| |_) | / / /  @
|_| |_|\__|\__| .__(_)_/_/   @
              |_|            ',
        'Well done! You’ve taken your first real steps into the world of network programming. Along the way,
you’ve:@
@
• Explored why building an HTTP server in C is a powerful way to learn how the internet works under the hood.@
• Gained a solid grasp of the OSI model and seen exactly where TCP/IP and HTTP fit in.@
• Learned to resolve hostnames and services into network addresses with getaddrinfo().@
• Created and bound sockets using socket() and bind() so your server can claim its place on the network.@
• Accepted incoming connections with listen() and accept(), handing each client its own socket.@
• Read HTTP requests and sent proper HTTP/1.1 responses with recv() and send(), complete with minimal HTML content.@
• Combined all of this into a functional, minimal web server that you fully understand from top to bottom.@
@
You now have the foundations to move on to the next course, where we’ll build on this knowledge to create more
powerful and feature-rich network applications.'
    );

INSERT
    OR IGNORE INTO courses (
        name,
        short_desc,
        ascii_logo,
        end_of_course_msg
    )
VALUES
    (
        'Parsing HTTP for Your C Server',
        'Learn to build an HTTP parser in C to enhance your HTTP server,
dissecting messages into components for robust request handling in this
follow-up course.',
        "     _  _ _____ _____ ___ @
    | || |_   _|_   _| _ \@
 ___| __ | | |   | | |  _/@
| _ \_||_|_|_|___|_| |_|  @
|  _/ _` | '_(_-/ -_| '_| @
|_| \__,_|_| /__\___|_|   ",
        ''
    );

INSERT
    OR IGNORE INTO courses (
        name,
        short_desc,
        ascii_logo,
        end_of_course_msg
    )
VALUES
    (
        'Handling multiple clients',
        'Master managing multiple clients in your HTTP server, implementing
concurrent connections using techniques like threading to boost performance
and scalability in this follow-up course. ',
        '[CLIENT]\  +----+  /[CLIENT]@
         \ |=  o| /         @
          \|----|/          @
          /|=  o|\          @
         / +----+ \         @
[CLIENT]/  SERVER  \[CLIENT]',
        ''
    );

INSERT
    OR IGNORE INTO courses (
        name,
        short_desc,
        ascii_logo,
        end_of_course_msg
    )
VALUES
    (
        'Secure communication with SSL',
        'Learn to secure your server with SSL/TLS, implementing encrypted
communication to protect data transfers and ensure secure client-server
interactions in this follow-up course.',
        '   _____     ____ ____  _     @
  /     \   / ___/ ___|| |    @
+---------+ \___ \___ \| |    @
|    0    |  ___) ___) | |___ @
|    |    | |____|____/|_____|@
+---------+     ENCRYPTION    ',
        ''
    );
