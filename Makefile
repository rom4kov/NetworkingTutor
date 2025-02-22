CC = gcc
CFLAGS = -g -Wall -Wextra -std=c17
LDFLAGS = -lpanel -lmenu -lform -lsqlite3 -lncurses 
INC=-Isrc/

TARGET = main

SRC = $(wildcard *.c) $(wildcard src/**/*.c)

$(TARGET): $(SRC)
		$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
