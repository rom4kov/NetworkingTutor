CREATE TABLE IF NOT EXISTS progress (
  id INTEGER PRIMARY KEY,
  user_id int NOT NULL,
  course_id int NOT NULL,
  sections_completed int,
  items_completed int
)
