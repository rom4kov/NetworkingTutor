CC = gcc
CFLAGS = -ggdb -Wall -Wextra -std=c17
LDFLAGS = -lpanel -lmenu -lform -lsqlite3 -lncurses 
INC=-Isrc/

TARGET = main

SRC = main.c src/views/start_screen.c src/views/user_form.c src/views/course_view.c src/data/data_access_layer.c src/core/input_handler.c src/core/helpers.c 

$(TARGET): $(SRC)
		$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
