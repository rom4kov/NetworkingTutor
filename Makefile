CC = gcc
CFLAGS = -g -Wall -Wextra -std=c17 -DPCRE2_CODE_UNIT_WIDTH=8 -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lpanelw -lmenuw -lformw -lsqlite3 -lpcre2-8 -lncursesw -lcunit -lm
INC=-Isrc/

TARGET = main

SRC = $(wildcard *.c) $(wildcard src/**/*.c) $(wildcard src/**/**/*.c)

$(TARGET): $(SRC)
		$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
