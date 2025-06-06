CREATE TABLE IF NOT EXISTS materials (
    id INTEGER PRIMARY KEY,
    course_id INTEGER NOT NULL,
    section_title TEXT,
    section_id INTEGER NOT NULL,
    content TEXT,
    order_num INTEGER,
    FOREIGN KEY(course_id) REFERENCES courses(id)
);

INSERT
    OR IGNORE INTO materials (
        course_id,
        section_title,
        section_id,
        content,
        order_num
    )
VALUES
    (
        1,
        'Intro',
        0,
        '  _     _   _             
____\n 
| |__ | |_| |_ _ __ _   / / /\n | `_ \\| __| __| `_ (_) / / /\n | | | 
| |_| |_| |_) | / / / \n |_| |_|\\__|\\__| .__(_)_/_/  \n             
  |_|         
  \n',
        0
    );

INSERT
    OR IGNORE INTO materials (
        course_id,
        section_title,
        section_id,
        content,
        order_num
    )
VALUES
    (
        1,
        'Intro',
        0,
        'So why of all things build an HTTP server as the a project 
to learn network progrmming? There are many good reasons for doing so,
but here we want to emphasize three in pariticular:',
        1
    );

INSERT
    OR IGNORE INTO materials (
        course_id,
        section_title,
        section_id,
        content,
        order_num
    )
VALUES
    (
        1,
        'Intro',
        0,
        'To get 
started, there are a few things we have to talk about so that we 
are on the same page as to what it is we are actually dealing with.',
        2
    );
