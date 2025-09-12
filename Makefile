CC = gcc
CFLAGS = -g -Wall -Wextra -std=c17 -O1
LDFLAGS = -lpanel -lmenu -lform -lsqlite3 -lpcre2-8 -lncursesw -lcunit -lm
INC=-Isrc/

TARGET = main

SRC = $(wildcard *.c) $(wildcard src/**/*.c) $(wildcard src/**/**/*.c)

$(TARGET): $(SRC)
		$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
