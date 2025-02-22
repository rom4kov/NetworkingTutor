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
    int gap_size = 64;

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
    rewind(file);

    do
    {
        c = fgetc(file);
        if (c == '\n')
            num_lines++;
    } while (c != EOF);
    rewind(file);

    mvwprintw(edit_window, LINES - 7, 2, "File size: %ib", file_size);
    mvwprintw(edit_window, LINES - 7, 22, "Lines: %i", num_lines);
    wrefresh(edit_window);

    if (file != NULL)
    {
        int i, j, k;
        i = j = k = 0;

        char_buf.buf_ = calloc(file_size + gap_size, sizeof(char));
        char_buf.ccur_ = char_buf.buf_;
        char_buf.cend_ = char_buf.ccur_ + gap_size;
        char_buf.size_ = gap_size;
        char_buf.last_mod_ = char_buf.buf_;

        line_buf.line_size_ = calloc(num_lines, sizeof(int));
        line_buf.ccur_ = 0;
        line_buf.cend_ = 0;
        line_buf.size_ = 0;

        while (fread(&c, sizeof(char), 1, file))
        {
            mvwprintw(edit_window, 1, i, "%c", c);
            if (c == '\n')
            {
                char_buf.buf_[char_buf.size_] = c;
                char_buf.size_++;
                if (line_buf.size_ == 0)
                {
                    line_buf.line_size_[line_buf.size_] =
                        char_buf.size_ - gap_size;
                }
                else
                {
                    prev_chars = 0;
                    for (j = 0; j < line_buf.size_; j++)
                        prev_chars += line_buf.line_size_[j];
                    line_buf.line_size_[line_buf.size_] =
                        char_buf.size_ - gap_size - prev_chars;
                }
                line_buf.size_++;
                continue;
            }
            char_buf.buf_[char_buf.size_] = c;
            char_buf.size_++;
            i++;
        }

        rewind(file);

        mvwprintw(edit_window, 1, 0, "%s", char_buf.buf_);
        wattron(windows[1], A_BOLD);
        mvwprintw(windows[1], 1, 1, "%s", filename);
        wattroff(windows[1], A_BOLD);
        for (i = 0; i < line_buf.size_; i++)
        {
            if (i < 9)
            {
                wattron(line_num_win, COLOR_PAIR(4));
                mvwprintw(line_num_win, i + 1, 1, "%i", i + 1);
                wattron(line_num_win, COLOR_PAIR(4));
            }
            else
            {
                wattron(line_num_win, COLOR_PAIR(4));
                mvwprintw(line_num_win, i + 1, 0, "%i", i + 1);
                wattron(line_num_win, COLOR_PAIR(4));
            }
            for (j = 0; j < line_buf.line_size_[i]; j++)
            {
                mvwprintw(edit_window, i, j, "%c", char_buf.buf_[k + gap_size]);
                k++;
            }
        }
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
                        mvwprintw(windows[5], 30, 2, "active window: %i",
                                  *active_win);
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
            handle_editor_input(ch, edit_window, y, x, &char_buf, &line_buf,
                                gap_size, &editor_mode, file, file_size + gap_size);
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

//
// void handle_editor_input(int ch, WINDOW **edit_window, int y, int x,
//                          CHAR_BUFFER *char_buf, LINE_BUFFER *line_buf,
//                          int gap_size, bool *editor_mode)
// {
//     switch (ch)
//     {
//         case KEY_RIGHT:
//             char_buf->ccur_++;
//             char_buf->cend_++;
//             getyx(*edit_window, y, x);
//             wmove(*edit_window, y, x + 1);
//             wrefresh(*edit_window);
//             break;
//         case KEY_LEFT:
//             char_buf->ccur_--;
//             char_buf->cend_--;
//             getyx(*edit_window, y, x);
//             wmove(*edit_window, y, x - 1);
//             wrefresh(*edit_window);
//             break;
//         case KEY_DOWN:
//             char_buf->ccur_ += line_buf->line_size_[line_buf->ccur_];
//             char_buf->cend_ += line_buf->line_size_[line_buf->ccur_];
//             line_buf->ccur_++;
//             getyx(*edit_window, y, x);
//             wmove(*edit_window, y + 1, x);
//             wrefresh(*edit_window);
//             break;
//         case KEY_UP:
//             char_buf->ccur_ -= line_buf->line_size_[line_buf->ccur_ - 1];
//             char_buf->cend_ -= line_buf->line_size_[line_buf->ccur_ - 1];
//             line_buf->ccur_--;
//             getyx(*edit_window, y, x);
//             wmove(*edit_window, y - 1, x);
//             wrefresh(*edit_window);
//             break;
//         case KEY_BACKSPACE:
//             char_buf->ccur_--;
//             gap_size++;
//             getyx(*edit_window, y, x);
//             if (x == 0)
//             {
//                 wmove(*edit_window, y - 1, x);
//                 wrefresh(*edit_window);
//                 break;
//             }
//             mvwprintw(*edit_window, y, x - 1, " ");
//             wmove(*edit_window, y, x - 1);
//             wrefresh(*edit_window);
//             break;
//         case 10:
//             *char_buf->ccur_ = '\n';
//             gap_size--;
//             getyx(*edit_window, y, x);
//             wprintw(*edit_window, "%c", '\n');
//             wmove(*edit_window, y + 1, 0);
//             wrefresh(*edit_window);
//             break;
//         case KEY_F(1):
//             curs_set(0);
//             wrefresh(*edit_window);
//             *editor_mode = false;
//             break;
//         default:
//             memcpy(char_buf->ccur_, char_buf->cend_, (*char_buf->ccur_ -
//             *char_buf->buf_)); *char_buf->ccur_ = ch; char_buf->ccur_++;
//             gap_size--;
//             line_buf->line_size_[line_buf->ccur_]++;
//             wprintw(*edit_window, "%c", ch);
//             // wmove(*edit_window, y, x + 1);
//             wrefresh(*edit_window);
//             break;
//     }
// }
