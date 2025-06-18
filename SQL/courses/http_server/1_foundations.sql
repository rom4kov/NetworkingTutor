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
building an HTTP server we first need to have a basic mental model of how the
web actually works, what techniques and protocols it uses so that we can
communicate and send content around the globe.",
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
        "The people who first built and created what we know today as the internet
also came up with different models of who it works. The most standard one of these
models is the OSI (Open Systems Interconnection) model. It describes 7 layers each
each of which make up a specific phase that is traversed when a data is send from
one computer to another:",
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
        "
Layer | Name         | Protocols  | PDU     | Purpose@
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
        "Every time when you connect to the internet, go to a website, check your
emails or join a zoom call, data moves from your computer to another computer somewhere
far away. And every piece of data that does this moves through all of these layers,
from top to bottom, from layer 7 to layer 1, when it gets send from your machine,
and then then other way around, from bottom to top, when the data is received at the
other end by a different computer.",
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
        "On each layer exist many different protocols that define how particalur kinds
of application data is handled at that stage of the flow of data through the network stack.
An HTTP server operates on three of these seven layers: layer 3, 4 and 7.",
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
        "Let's start with layer 3, the network layer. The purpose of this layer is to allow 
data to be routed correctly from the source computer to the destination computer.
The most used protocol entrusted with this task is IP, the Internet Protocol. It's main job is
is to tell any router the data passes, where the packet comes from and where it is headed by 
adding a header to the data packet that specifies two addresses for the source and the 
destination.",
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
        "Next comes layer 4, the transport layer. It deals with challenges that can occur with data
transmission like connection relialibilty and adequate order, rate and amount of data units 
being passed through the network. Almost all traffic on the internet is handled by two layer 4 
protocols, TCP and UDP.",
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
        "TCP (Transmission Control Protocol) is responsible for three things above all: establishing a stable
and reliable connection between two computers, accounting for lost packets and ensuring their correct order
when larger data units are split up in multiple packets, and last not least specifying what service or
application on each computer the data is meant for or coming from.",
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
        "UDP is another common protocol on layer 4 which does not establish a synchronized and reliable connection
like TCP does and is used for example for video stream data where losing a couple of packets is not a big deal.
An HTTP server commonly uses TCP (unless it's HTTP/3, the most recent but not yet standard version of HTTP)'),
because a reliable and correctly ordered transmission of data is essential for web content.",
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
        "The last protocol relevant for building an HTTP server is located at layer 7 and its name is probably 
(you already guessed it) HTTP (Hypertext Transfer Protocol). It is used for the transfer of hypermedia like 
data web pages, images, and video and specifies a range of request methods and header options that enable and
simplify the exchange of said data.",
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
        "This is of course a very condensed overview to give you some very general context. We will revisit and
refer back to these topics in the sections when it will be appropriate and help us to understand the bigger picture
of all the smaller steps necessary to build an HTTP server. So let's get to it!",
        10
    );
