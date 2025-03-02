#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_COUNT 3
#define WU COLS / 12 // WU for WIDTH_UNIT

void handle_course_input(WINDOW **windows, int *active_win, MENU **start_menu,
                         sqlite3 *db)
{
    bool in_course_view = true;
    bool editor_mode = false;
    int ch;
    ITEM *curr_item;
    int y, x, num_lines, prev_chars;
    y = x = num_lines = prev_chars = 0;
    char c;

    WINDOW *line_num_win = derwin(windows[1], LINES - 5, 3, 1, 1);
    WINDOW *edit_window = derwin(windows[1], LINES - 6, WU * 7 - 2, 2, 4);
    // FIELD *fields[3];

    CHAR_BUFFER char_buf;
    LINE_BUFFER line_buf;
    // TEXT_EDITOR editor;

    char *filename = "../hello.c";
    FILE *file = fopen(filename, "r+");

    if (file == NULL)
    {
        printf("Could not open %s.\n", filename);
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    int new_file_size = file_size;
    rewind(file);

    do
    {
        c = fgetc(file);
        if (c == '\n')
            num_lines++;
    } while (c != EOF);
    rewind(file);

    if (file != NULL)
    {
        int j;
        j = 0;

        char_buf.gap_size_ = 64;
        char_buf.buf_ = calloc(file_size + char_buf.gap_size_, sizeof(char));
        char_buf.ccur_ = char_buf.buf_;
        char_buf.cend_ = char_buf.ccur_ + char_buf.gap_size_;
        char_buf.size_ = char_buf.gap_size_;
        char_buf.mod_size_ = 0;

        line_buf.gap_size_ = 64;
        line_buf.line_size_ = calloc(num_lines, sizeof(int));
        line_buf.new_lines_ =
            calloc(num_lines + line_buf.gap_size_, sizeof(int));
        line_buf.ccur_ = line_buf.new_lines_;
        line_buf.cend_ = line_buf.ccur_ + line_buf.gap_size_;
        line_buf.mod_size_ = 0;
        line_buf.size_ = 0;

        while (fread(&c, sizeof(char), 1, file))
        {
            // mvwprintw(edit_window, 1, i, "%c", c);
            if (c == '\n')
            {
                char_buf.buf_[char_buf.size_] = c;
                line_buf.new_lines_[line_buf.size_ + line_buf.gap_size_] =
                    char_buf.size_ - char_buf.gap_size_;
                char_buf.size_++;
                if (line_buf.size_ == 0)
                {
                    line_buf.line_size_[line_buf.size_] =
                        char_buf.size_ - char_buf.gap_size_;
                }
                else
                {
                    prev_chars = 0;
                    for (j = 0; j < line_buf.size_; j++)
                        prev_chars += line_buf.line_size_[j];
                    line_buf.line_size_[line_buf.size_] =
                        char_buf.size_ - char_buf.gap_size_ - prev_chars;
                }
                line_buf.size_++;
                continue;
            }
            char_buf.buf_[char_buf.size_] = c;
            char_buf.size_++;
        }

        rewind(file);

        // mvwprintw(edit_window, 1, 0, "%s", char_buf.buf_);
        wattron(windows[1], A_BOLD);
        mvwprintw(windows[1], 1, 1, "%s", filename);
        wattroff(windows[1], A_BOLD);
        wrefresh(edit_window);
        update_edit_window(&char_buf, &line_buf, &new_file_size, line_num_win,
                           edit_window);
        mvwprintw(edit_window, LINES - 14, 2, "Before");
        mvwprintw(edit_window, LINES - 13, 2, "First byte of buffer: %p",
                  char_buf.buf_);
        mvwprintw(edit_window, LINES - 12, 2, "ccur: %p", char_buf.ccur_);
        mvwprintw(edit_window, LINES - 11, 2, "cend: %p", char_buf.cend_);

        int first_buf_len = char_buf.ccur_ - char_buf.buf_;
        mvwprintw(edit_window, LINES - 10, 2, "First buffer length: %i",
                  first_buf_len);
        int second_buf_len = &char_buf.buf_[file_size] - char_buf.cend_;
        mvwprintw(edit_window, LINES - 9, 2, "Second buffer length: %i",
                  second_buf_len);
        wrefresh(edit_window);
    }

    wrefresh(line_num_win);
    wrefresh(edit_window);
    wrefresh(windows[1]);

    while (in_course_view)
    {
        ch = getch();
        ESCDELAY = 100;

        if (ch == 27)
            return;

        if (*active_win == 0)
        {
            switch (ch)
            {
                case KEY_LEFT:
                    menu_driver(*start_menu, REQ_PREV_ITEM);
                    wrefresh(windows[0]);
                    break;
                case KEY_RIGHT:
                    menu_driver(*start_menu, REQ_NEXT_ITEM);
                    wrefresh(windows[0]);
                    break;
                case 9:
                case KEY_DOWN:
                    *active_win = 1;
                    focus_window(windows, 0, 2, active_win, "Navigation");
                    focus_window(windows, 1, 3, active_win, "Editor");
                    break;
                case '\n':
                    curr_item = current_item(*start_menu);
                    if (item_index(curr_item) == 0)
                    {
                        in_course_view = false;
                        for (int i = 0; i < WINDOW_COUNT; ++i)
                        {
                            // delwin(windows[i]);
                            wclear(windows[i]);
                        }
                        create_start_screen(db);
                        break;
                    }
                    else if (item_index(curr_item) == 2)
                    {
                        focus_window(windows, 0, 2, active_win, "Navigation");
                        focus_window(windows, 5, 3, active_win, "Details");
                        *active_win = 5;
                        // mvwprintw(windows[5], 30, 2, "active window: %i",
                        //           *active_win);
                        // create_user_form(&windows[5], &user_form, fields);
                        focus_window(windows, 5, 3, active_win, "Details");
                        wmove(windows[5], 4, 14);
                        wrefresh(windows[5]);
                    }
                    break;
            }
        }
        else if (*active_win == 1 && editor_mode)
        {
            handle_editor_input(ch, &line_num_win, &edit_window, y, x,
                                &char_buf, &line_buf, &editor_mode, file,
                                file_size + char_buf.gap_size_, &new_file_size);
            // fclose(file);
        }
        else if (*active_win == 1)
        {
            switch (ch)
            {
                case 9:
                case KEY_RIGHT:
                    *active_win = 2;
                    focus_window(windows, 1, 2, active_win, "Editor");
                    focus_window(windows, 2, 3, active_win,
                                 "Course Instructions");
                    break;
                case KEY_UP:
                    *active_win = 0;
                    focus_window(windows, 0, 3, active_win, "Navigation");
                    focus_window(windows, 1, 2, active_win, "Editor");
                    break;
                case 10:
                    editor_mode = true;
                    curs_set(1);
                    wmove(edit_window, 0, 0);
                    wrefresh(edit_window);
                    break;
            }
        }
        else if (*active_win == 2)
        {
            switch (ch)
            {
                case 9:
                case KEY_LEFT:
                    *active_win = 1;
                    focus_window(windows, 1, 3, active_win, "Editor");
                    focus_window(windows, 2, 2, active_win,
                                 "Course Instructions");
                    break;
            }
        }
    }

    free(char_buf.buf_);
    free(line_buf.line_size_);
}
