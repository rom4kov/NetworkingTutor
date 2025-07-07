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
to tell it what the address and port are on which it should listen for incoming 
connection requests and data.',
        10
    );
