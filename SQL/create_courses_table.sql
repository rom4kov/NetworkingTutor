CREATE TABLE IF NOT EXISTS courses (
  id int NOT NULL, 
  name varchar(255) NOT NULL, 
  short_desc varchar(255), PRIMARY KEY (id)
);

INSERT INTO courses 
VALUES(
  'Build an HTTP Server in C',
  'In this first and short project you will learn how to build a simple HTTP server in C,
  using the very basic version 1.0 of the HTTP protocol.'
);

INSERT INTO courses 
VALUES(
  'Handling multiple clients',
  'In the second project you will learn how to build a simple HTTP server in C,
  using the very basic version 1.0 of the HTTP protocol.'
);

INSERT INTO courses 
VALUES(
  'Secure communication with SSL',
  'In the second project you will learn how to build a simple HTTP server in C,
  using the very basic version 1.0 of the HTTP protocol.'
);
