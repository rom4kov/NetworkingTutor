CREATE TABLE IF NOT EXISTS sections (
    id INTEGER PRIMARY KEY,
    course_id INTEGER NOT NULL,
    section_title TEXT,
    num_of_items INTERGER NOT NULL,
    order_num INTEGER,
    has_test bool,
    FOREIGN KEY(course_id) REFERENCES courses(id) UNIQUE(course_id, section_title, order_num)
);
