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
        "",
        "
 Layer | Name         | Protocols    | PDU     | Purpose@
-------+--------------+--------------+---------+-----------------------------@
     7 | Application  | HTTP, FTP    | Data    | User apps like Web browsers@
     6 | Presentation | TLS          | Data    | Encode, encrypt, compress@
     5 | Session      | RPC          | Data    | Sessions, auth, sync@
     4 | Transport    | TCP, UDP     | Segment | Ports & transport@
     3 | Network      | IP, ICMP     | Packet  | Routing & addressing@
     2 | Data Link    | 802.11, WLAN | Frame   | MAC addressing@
     1 | Physical     | 1000BASE-T   | Bit     | Electrical transmission",
        2
    );
