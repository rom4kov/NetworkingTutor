CREATE TABLE IF NOT EXISTS progress (
  id int NOT NULL,
  user_id int NOT NULL,
  course_id int NOT NULL,
  sections_completed int NOT NULL,
  PRIMARY KEY(id)
)
