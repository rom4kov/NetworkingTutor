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
    SERVER    |_|            ',
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
        'Serving Static Files',
        'Learn how to extend your HTTP server to deliver real web content by reading
files from disk — such as HTML, CSS, or images — and sending them with the correct
headers to clients.',
        '+------+            +------+  @
|++  oo|            | HTML |  @
|------| ---------> | CSS  |  @
|++  oo|            | JPEG |  @
+------+            +------+  @
 SERVER           STATIC FILES',
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
        'Implementing a Simple HTTP Router',
        'Serve different content depending on the requested path. Map paths to
handler functions, write different routines for each endpoint and a robust and
reliable routing logic.',
        'REQUEST        ROUTE HANDLER@
[ GET / ] ---------> home() @
                            @
[ GET /hello ] ----> hello()@
                            @
[ GET /about ] ----> about()',
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


INSERT
    OR IGNORE INTO courses (
        name,
        short_desc,
        ascii_logo,
        end_of_course_msg
    )
VALUES
    (
        'Implementing Non-Blocking I/O',
        'Learn event-driven server design to handle many clients efficiently
without relying on threads, using system calls like select() or poll() for
scalable network programming.',
        '        0s        5s       10s@
         :         :         :@
Blocking :xxxxxxxxx:oooooooo :@
         :         :         :@
Non-     :xxxxxxxxx:         :@
Blocking :ooooooo  :         :',
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
        'Simple Auth (Cookies & Sessions)',
        'Introduce logins, sessions, and cookies, giving your server stateful
interaction with clients, enabling personalized responses and laying the groundwork
for secure user authentication.',
        '    _       +- Name ---------+@
  _(-)_     |John            |@
 /_____\    +----------------+@
 __         +- Password -----+@
/o \_____   |ooooooo         |@
\__/-="="`  +----------------+',
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
        'Building a Basic HTTP Client in C',
        'Switch sides: create your own HTTP client that connects to servers, sends
requests, and parses responses, learning how clients interact with web services.',
        ' _     _   _             ____@
| |__ | |_| |_ _ __ _   / / /@
| `_ \| __| __| `_ (_) / / / @
| | | | |_| |_| |_) | / / /  @
|_| |_|\__|\__| .__(_)_/_/   @
    CLIENT    |_|            ',
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
        'Chat Server with TCP Sockets',
        'Go beyond HTTP to raw TCP: build a real-time chat server where multiple
clients can message each other simultaneously, practicing persistent connections
and message broadcasting.',
        '                +------------+@
+-----------+   |    Yo!     |@
|  Nothing  |   |  What up?  |@
|   much!   |   +--------+ +-+@
+-+ +-------+             \|  @
  |/                          ',
        ''
    );
