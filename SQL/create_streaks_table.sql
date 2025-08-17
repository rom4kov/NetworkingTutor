CREATE TABLE IF NOT EXISTS streaks (
    id INTEGER PRIMARY KEY,
    user_id int NOT NULL,
    streak int,
    achieved_at DATETIME,
    UNIQUE(user_id, course_id, section_id)
)

