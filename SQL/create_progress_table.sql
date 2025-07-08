CREATE TABLE IF NOT EXISTS progress (
  id INTEGER PRIMARY KEY,
  user_id int NOT NULL,
  course_id int NOT NULL,
  section_id int,
  section_completed bool,
  items_completed int,
  UNIQUE(user_id, course_id, section_id)
)
