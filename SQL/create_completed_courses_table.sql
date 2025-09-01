CREATE TABLE completed_courses (
    id INTEGER PRIMARY KEY,
    user_id INTEGER NOT NULL,
    course_id INTEGER NOT NULL,
    completed_at DATETIME,
    PRIMARY KEY(user_id, course_id),
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY(course_id) REFERENCES courses(id)
);
