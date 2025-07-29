INSERT
    OR IGNORE INTO sections (
        course_id,
        section_title,
        num_of_items,
        order_num,
        has_test,
        has_separate_task
    )
VALUES
    (
        1,
        "1. Intro",
        8,
        0,
        0,
        0
    );

INSERT
    OR IGNORE INTO sections (
        course_id,
        section_title,
        num_of_items,
        order_num,
        has_test,
        has_separate_task
    )
VALUES
    (
        1,
        "2. Foundations: TCP/IP and HTTP",
        12,
        1,
        1,
        1
    );

INSERT
    OR IGNORE INTO sections (
        course_id,
        section_title,
        num_of_items,
        order_num,
        has_test,
        has_separate_task
    )
VALUES
    (
        1,
        "3. getaddrinfo() - how to get address information",
        12,
        2,
        1,
        0
    );

INSERT
    OR IGNORE INTO sections (
        course_id,
        section_title,
        num_of_items,
        order_num,
        has_test,
        has_separate_task
    )
VALUES
    (
        1,
        "4. Understanding socket(): Creating Communication Endpoints",
        12,
        3,
        1,
        0
    );
