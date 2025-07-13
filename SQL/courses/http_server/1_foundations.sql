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
        1,
        "",
        "In order to understand what we're actually trying to accomplish by
building an HTTP server we first need a basic mental model of how the web works,
what techniques and protocols it uses to move information around the globe.",
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
        1,
        "",
        "To make sense of these processes, early internet architects introduced
various models to describe how computer systems communicate. The most widely used
is the OSI model (Open Systems Interconnection). It breaks down communication 
into seven layers, each representing a distinct phase data goes through when
sent from one computer to another:",
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
        1,
        "The OSI model",
        "Layer | Name         | Protocols  | PDU     | Purpose@
------+--------------+------------+---------+--------------------------@
    7 | Application  | HTTP, FTP  | Data    | End-user services@
    6 | Presentation | TLS        | Data    | Encode, encrypt, compress@
    5 | Session      | RPC        | Data    | Sessions, auth, sync@
    4 | Transport    | TCP, UDP   | Segment | Ports & transport@
    3 | Network      | IP, ICMP   | Packet  | Routing & addressing@
    2 | Data Link    | Ethernet   | Frame   | MAC addressing@
    1 | Physical     | 1000BASE-T | Bit     | Electrical transmission",
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
        1,
        "",
        "Whenever you visit a website, send an email, or join a video call,
data travels from your device to a remote server—and it passes through all
of these layers. It goes top to bottom (Layer 7 to Layer 1) as it leaves your
machine, and bottom to top as it’s received on the other end.",
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
        1,
        "",
        "Each layer includes protocols that define how specific kinds of data
are handled at that stage. In our case, a simple HTTP server interacts primarily
with three layers: Layer 3, Layer 4, and Layer 7.",
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
        1,
        "Layer 3: The Network Layer",
        "The job of the network layer is to route data between devices across
different networks. The primary protocol used here is IP (Internet Protocol),
which attaches a header to each packet containing the source and destination IP
addresses. This allows routers to forward the packet toward its target.",
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
        1,
        "Layer 4: The Transport Layer",
        "Layer 4 is responsible for end-to-end transport of data between
applications. It enables communication between software running on different
machines, using different strategies depending on the protocol.",
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
        1,
        "",
        "The two main Layer 4 protocols are:@@TCP (Transmission Control Protocol)
— provides reliable, ordered delivery of data. It ensures packets arrive intact,
in order, and without duplication, using mechanisms like acknowledgments and
retransmission.",
        7
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
        1,
        "",
        "UDP (User Datagram Protocol) — offers a faster, connectionless alternative.
It doesn’t guarantee delivery or order, which makes it suitable for applications
like real-time video or gaming, where speed is more important than accuracy.@@
HTTP servers use TCP because it's essential that all web content is delivered 
completely and in the correct order.@",
        8
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
        1,
        "Layer 7: The Application Layer",
        "At the top, we find HTTP (Hypertext Transfer Protocol), the core protocol
for web communication. It defines the structure of requests and responses exchanged
between clients and servers. HTTP specifies request methods (like GET, POST) and
headers (like Content-Type, Content-Length) that tell the server what content is
requested and how it should respond.",
        9
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
        1,
        "",
        "This overview is intentionally brief and incomplete. We’ll refer back
to these concepts in later sections whenever they help make sense of what we’re
building. For now, just keep this high-level picture in mind. It’ll make the
journey of writing your own HTTP server in C much easier to navigate.",
        10
    );
