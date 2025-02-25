#define _XOPEN_SOURCE 500

#include "../models/models.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void write_buffer_to_file(CHAR_BUFFER *cbuf, FILE *file, int file_size,
                          int *new_file_size, WINDOW *edit_window, char *ccur,
                          char *cend, LINE_BUFFER *lbuf)
{
    for (int i = 0; i < lbuf->size_; i++)
    {
        mvwprintw(edit_window, LINES - 29 + i, 6, "%i", lbuf->line_size_[i]);
    }

    int first_buf_len = cbuf->ccur_ - cbuf->buf_;
    int second_buf_len = &cbuf->buf_[file_size] - cbuf->cend_;
    mvwprintw(edit_window, LINES - 16, 52, "After:");
    mvwprintw(edit_window, LINES - 15, 52, "Mod size: %i", cbuf->mod_size_);
    mvwprintw(edit_window, LINES - 14, 52, "File pointer: %li", ftell(file));
    mvwprintw(edit_window, LINES - 13, 52, "First byte of buffer: %p",
              cbuf->buf_);
    mvwprintw(edit_window, LINES - 12, 52, "ccur: %p", ccur);
    mvwprintw(edit_window, LINES - 11, 52, "cend: %p", cend);

    mvwprintw(edit_window, LINES - 10, 52, "First buffer length: %i",
              first_buf_len);
    mvwprintw(edit_window, LINES - 9, 52, "Second buffer length: %i",
              second_buf_len);

    fwrite(cbuf->buf_, sizeof(char), first_buf_len, file);
    fwrite(cend, sizeof(char), second_buf_len, file);

    rewind(file);

    int chars_diff = *new_file_size - file_size;
    mvwprintw(edit_window, LINES - 20, 52, "file_size: %i", file_size);
    mvwprintw(edit_window, LINES - 21, 52, "new_file_size: %i", *new_file_size);
    mvwprintw(edit_window, LINES - 22, 52, "chars_diff: %i", chars_diff);
    fseek(file, chars_diff, SEEK_END);
    off_t position = ftell(file);
    mvwprintw(edit_window, LINES - 24, 52, "position: %li", position);
    // const char *path = "../hello.c";
    int rc;
    if ((rc = ftruncate(fileno(file), position)) == 0)
    {
        mvwprintw(edit_window, LINES - 16, 70, "Truncated");
    }
    else
    {
        mvwprintw(edit_window, LINES - 16, 70, "Error: %i", rc);
    }

    wrefresh(edit_window);
}

void update_edit_window(CHAR_BUFFER *cbuf, LINE_BUFFER *lbuf, int *gap_size,
                        WINDOW *line_num_win, WINDOW *edit_window)
{
    wclear(edit_window);
    int i, j, k, l;
    k = 0;
    int file_size = 144;

    int first_buf_len = cbuf->ccur_ - cbuf->buf_;
    int start_second_buf = first_buf_len + *gap_size;
    int second_buf_len = &cbuf->buf_[file_size] - cbuf->cend_;

    mvwprintw(edit_window, LINES - 14, 2, "Before");
    mvwprintw(edit_window, LINES - 13, 23, "%s", "  ");
    mvwprintw(edit_window, LINES - 13, 2, "First byte of buffer: %p",
              cbuf->buf_);
    mvwprintw(edit_window, LINES - 12, 8, "%s", "  ");
    mvwprintw(edit_window, LINES - 12, 2, "ccur: %p", cbuf->ccur_);
    mvwprintw(edit_window, LINES - 11, 8, "%s", "  ");
    mvwprintw(edit_window, LINES - 11, 2, "cend: %p", cbuf->cend_);

    mvwprintw(edit_window, LINES - 10, 23, "%s", "  ");
    mvwprintw(edit_window, LINES - 10, 2, "First buffer length: %i",
              first_buf_len);
    mvwprintw(edit_window, LINES - 9, 2, "Second buffer length: %i",
              second_buf_len);

    mvwprintw(edit_window, LINES - 16, 40, "%s", "  ");
    mvwprintw(edit_window, LINES - 16, 40, "%i", first_buf_len);
    mvwprintw(edit_window, LINES - 15, 40, "%s", "  ");
    mvwprintw(edit_window, LINES - 15, 40, "%i", *gap_size);
    mvwprintw(edit_window, LINES - 14, 40, "%s", "  ");
    mvwprintw(edit_window, LINES - 14, 40, "%i", start_second_buf);
    mvwprintw(edit_window, LINES - 13, 40, "%s", "  ");
    mvwprintw(edit_window, LINES - 13, 40, "%i", second_buf_len);
    for (i = 0; i < lbuf->size_; i++)
    {
        // mvwprintw(edit_window, i + 14, k + 35 - i * 2, "%i", i);
        l = 0;
        mvwprintw(edit_window, LINES - 20 + i, 105, "%i", lbuf->new_lines_[i]);
        if (i < 9)
        {
            wattron(line_num_win, COLOR_PAIR(4));
            mvwprintw(line_num_win, i + 1, 1, "%i", i + 1);
            wattron(line_num_win, COLOR_PAIR(4));
            mvwprintw(edit_window, LINES - 29 + i, 2, "%i",
                      lbuf->line_size_[i]);
        }
        else
        {
            wattron(line_num_win, COLOR_PAIR(4));
            mvwprintw(line_num_win, i + 1, 0, "%i", i + 1);
            wattron(line_num_win, COLOR_PAIR(4));
            mvwprintw(edit_window, LINES - 29 + i, 2, "%i",
                      lbuf->line_size_[i]);
        }
        for (j = 0; j < lbuf->line_size_[i]; j++)
        {
            if (k == first_buf_len)
            {
                k = first_buf_len + *gap_size;
                mvwprintw(edit_window, i, j, "%c", cbuf->buf_[k]);
            }
            else
                mvwprintw(edit_window, i, j, "%c", cbuf->buf_[k]);
            mvwprintw(edit_window, i + 15, l + 10, "%i", k);
            k++;
            l += 4;
        }
    }
    // for (i = 0; i < lbuf->size_; i++)
    // {
    //     if (i < 9)
    //         mvwprintw(*line_num_win, i + 1, 1, "%i", i + 1);
    //     else
    //         mvwprintw(*line_num_win, i + 1, 0, "%i", i + 1);
    //     for (j = 0; j < lbuf->line_size_[i]; j++)
    //     {
    //         mvwprintw(*edit_window, i, j, "%c", cbuf->buf_[k + *gap_size]);
    //         k++;
    //     }
    // }
}

void handle_editor_input(int ch, WINDOW *line_num_win, WINDOW *edit_window,
                         int y, int x, CHAR_BUFFER *char_buf,
                         LINE_BUFFER *line_buf, int *gap_size,
                         bool *editor_mode, FILE *file, int file_size,
                         int *new_file_size)
{
    int curr_line_len = line_buf->line_size_[line_buf->ccur_];
    int next_line_len = line_buf->line_size_[line_buf->ccur_ + 1];
    int prev_line_len = line_buf->line_size_[line_buf->ccur_ - 1];
    int ccur_change_kdown;
    int ccur_change_kup;

    switch (ch)
    {
        case KEY_RIGHT:
            getyx(edit_window, y, x);
            if (x + 1 < curr_line_len - 1)
            {
                mvwprintw(edit_window, LINES - 7, 70, "             ");
                mvwprintw(edit_window, LINES - 7, 70, "Curr line: %i",
                          curr_line_len);
                mvwprintw(edit_window, LINES - 7, 90, "%s",
                          "                 ");
                mvwprintw(edit_window, LINES - 7, 90, "y: %i", y);
                mvwprintw(edit_window, LINES - 7, 96, "x: %i", x + 1);
                wmove(edit_window, y, x);
                char_buf->ccur_++;
                char_buf->cend_++;
                char_buf->mod_size_++;
                wmove(edit_window, y, x + 1);
                wrefresh(edit_window);
            }
            break;
        case KEY_LEFT:
            getyx(edit_window, y, x);
            if (x > 0)
            {
                mvwprintw(edit_window, LINES - 7, 70, "             ");
                mvwprintw(edit_window, LINES - 7, 70, "Curr line: %i",
                          curr_line_len);
                mvwprintw(edit_window, LINES - 7, 90, "%s",
                          "                 ");
                mvwprintw(edit_window, LINES - 7, 90, "y: %i", y);
                mvwprintw(edit_window, LINES - 7, 96, "x: %i", x - 1);
                wmove(edit_window, y, x);
                char_buf->ccur_--;
                char_buf->cend_--;
                char_buf->mod_size_--;
                wmove(edit_window, y, x - 1);
                wrefresh(edit_window);
            }
            break;
        case KEY_DOWN:
            if (line_buf->ccur_ - 1 < line_buf->size_)
            {
                getyx(edit_window, y, x);
                if (next_line_len > curr_line_len)
                    ccur_change_kdown = curr_line_len;
                else
                {
                    ccur_change_kdown =
                        line_buf->new_lines_[line_buf->ccur_ + 1] -
                                    (char_buf->ccur_ - char_buf->buf_) -
                                    (next_line_len < 2 ? 0 : 1);
                    mvwprintw(edit_window, LINES - 8, 102, "nl idx: %i",
                              line_buf->new_lines_[line_buf->ccur_ + 1]);
                    mvwprintw(edit_window, LINES - 8, 90, "curs: %li",
                              char_buf->ccur_ - char_buf->buf_);
                    mvwprintw(edit_window, LINES - 8, 72, "c mov: %i",
                              ccur_change_kdown);
                }

                // if (next_line_len > x && next_line_len > 2)
                // {
                //     ccur_change_kdown = curr_line_len;
                // }
                // else if (next_line_len > x && next_line_len <= 2)
                // {
                //     ccur_change_kdown = curr_line_len - 1;
                // }
                // else if (next_line_len == x && next_line_len > 1)
                // {
                //     ccur_change_kdown = curr_line_len - x;
                // }
                // else if (next_line_len < x && next_line_len > 2)
                // {
                //     ccur_change_kdown = curr_line_len - x + next_line_len -
                //     1;
                // }
                // else
                // {
                //     ccur_change_kdown = curr_line_len - x - 1;
                // }
                //        19                1        0         16          1   1
                //        1
                // ccur_change_kdown = (curr_line_len - x) + (next_line_len < 3
                // ? 0 : next_line_len - 1); ccur_change_kdown = curr_line_len -
                // x;
                mvwprintw(edit_window, LINES - 7, 70, "             ");
                mvwprintw(edit_window, LINES - 7, 70, "Curr line: %i",
                          next_line_len);
                mvwprintw(edit_window, LINES - 7, 90, "%s",
                          "                 ");
                mvwprintw(edit_window, LINES - 7, 90, "y: %i", y + 1);
                mvwprintw(edit_window, LINES - 7, 96, "x: %i", x);
                mvwprintw(edit_window, LINES - 7, 102, "          ");
                mvwprintw(edit_window, LINES - 7, 102, "c mov: %i",
                          ccur_change_kdown);
                wmove(edit_window, y, x);
                char_buf->ccur_ += ccur_change_kdown;
                char_buf->cend_ += ccur_change_kdown;
                char_buf->mod_size_ += ccur_change_kdown;
                line_buf->ccur_++;
                if (next_line_len > x)
                    wmove(edit_window, y + 1, x);
                else
                    wmove(edit_window, y + 1,
                          next_line_len == 1 ? next_line_len - 1
                                             : next_line_len - 2);
                wrefresh(edit_window);
            }
            break;
        case KEY_UP:
            if (line_buf->ccur_ > 0)
            {
                getyx(edit_window, y, x);
                if (prev_line_len > curr_line_len) ccur_change_kup = prev_line_len;
                else
                {
                    ccur_change_kup = (char_buf->ccur_ - char_buf->buf_) -
                                line_buf->new_lines_[line_buf->ccur_ - 1] +
                                (prev_line_len < 2 ? 1 : 0);
                }
                //     19                1         0          19         1   1 1
                // if (prev_line_len > x && prev_line_len > 1)
                // {
                //     ccur_change_kup = prev_line_len;
                // }
                // else if (prev_line_len <= x && prev_line_len > 1)
                // {
                //     ccur_change_kup = x + 2;
                //     // } else if (prev_line_len == x && prev_line_len == 1) {
                //     //     ccur_change_kup = 1;
                //     // } else if (prev_line_len < x) {
                //     //     ccur_change_kup = x + 2;
                // }
                // else
                // {
                //     ccur_change_kup = x + 1;
                // }
                // ccur_change_kup = x + (prev_line_len < 3 ? 1 : prev_line_len
                // - 1);
                mvwprintw(edit_window, LINES - 7, 70, "             ");
                mvwprintw(edit_window, LINES - 7, 70, "Curr line: %i",
                          prev_line_len);
                mvwprintw(edit_window, LINES - 7, 90, "%s",
                          "                 ");
                mvwprintw(edit_window, LINES - 7, 90, "y: %i", y - 1);
                mvwprintw(edit_window, LINES - 7, 96, "x: %i", x);
                mvwprintw(edit_window, LINES - 7, 102, "          ");
                mvwprintw(edit_window, LINES - 7, 102, "c mov: %i",
                          ccur_change_kup);
                wmove(edit_window, y, x);
                char_buf->ccur_ -= ccur_change_kup;
                char_buf->cend_ -= ccur_change_kup;
                char_buf->mod_size_ -= ccur_change_kup;
                line_buf->ccur_--;
                if (prev_line_len > x && prev_line_len > 2)
                    wmove(edit_window, y - 1, x);
                else if (prev_line_len <= x && prev_line_len > 2)
                    wmove(edit_window, y - 1, prev_line_len - 2);
                else
                    wmove(edit_window, y - 1, 0);
                wrefresh(edit_window);
            }
            break;
        case KEY_BACKSPACE:
            if (char_buf->mod_size_ > 0)
                memcpy(char_buf->ccur_ - char_buf->mod_size_,
                       char_buf->cend_ - char_buf->mod_size_,
                       char_buf->mod_size_);
            else if (char_buf->mod_size_ < 0)
                memcpy(char_buf->cend_, char_buf->ccur_,
                       abs(char_buf->mod_size_));
            char_buf->mod_size_ = 0;
            char_buf->ccur_--;
            *char_buf->ccur_ = ' ';
            *gap_size += 1;
            line_buf->line_size_[line_buf->ccur_] =
                line_buf->line_size_[line_buf->ccur_] - 1;
            *new_file_size -= 1;
            // mvwprintw(edit_window, LINES - 28, 52, "nfs after change: %i",
            // new_file_size);
            getyx(edit_window, y, x);
            if (x == 0)
            {
                wmove(edit_window, y - 1, x);
                wrefresh(edit_window);
                break;
            }
            // *chars = line_buf->line_size_[line_buf->ccur_] - x;
            // char *shift_str = malloc(*chars * sizeof(char));
            // memcpy(shift_str, char_buf->cend_,
            //        line_buf->line_size_[line_buf->ccur_] - x);
            // mvwprintw(edit_window, y, x - 1, "%s ", shift_str);
            update_edit_window(char_buf, line_buf, gap_size, line_num_win,
                               edit_window);
            wmove(edit_window, y, x - 1);
            wrefresh(edit_window);
            // free(shift_str);
            break;
        case 10:
            *char_buf->ccur_ = '\n';
            gap_size--;
            new_file_size++;
            getyx(edit_window, y, x);
            wprintw(edit_window, "%c", '\n');
            wmove(edit_window, y + 1, 0);
            wrefresh(edit_window);
            break;
        case KEY_F(1):
            curs_set(0);
            wrefresh(edit_window);
            *editor_mode = false;
            break;
        case KEY_F(10):
            mvwprintw(edit_window, LINES - 30, 52, "nfs before passing: %i",
                      *new_file_size);
            write_buffer_to_file(char_buf, file, file_size, new_file_size,
                                 edit_window, char_buf->ccur_, char_buf->cend_,
                                 line_buf);
            break;
        default:
            if (char_buf->mod_size_ > 0)
                memcpy(char_buf->ccur_ - char_buf->mod_size_,
                       char_buf->cend_ - char_buf->mod_size_,
                       char_buf->mod_size_);
            else if (char_buf->mod_size_ < 0)
                memcpy(char_buf->cend_, char_buf->ccur_,
                       abs(char_buf->mod_size_));
            *char_buf->ccur_ = ch;
            char_buf->ccur_++;
            *gap_size -= 1;
            line_buf->line_size_[line_buf->ccur_] =
                line_buf->line_size_[line_buf->ccur_] + 1;
            *new_file_size += 1;
            getyx(edit_window, y, x);
            update_edit_window(char_buf, line_buf, gap_size, line_num_win,
                               edit_window);
            // *chars = line_buf->line_size_[line_buf->ccur_] - x;
            // char *insert_str = malloc(*chars * sizeof(char));
            // memcpy(insert_str, char_buf->cend_,
            //        line_buf->line_size_[line_buf->ccur_] - x + 2);
            // wprintw(edit_window, "%c%s", ch, insert_str);
            // wprintw(edit_window, "%c", ch);
            // mvwprintw(edit_window, y, 40, "%c%i", ch, *chars);
            // mvwprintw(edit_window, y + 1, 40, "%i",
            //           line_buf->line_size_[line_buf->ccur_]);
            // free(insert_str);
            wmove(edit_window, y, x + 1);
            // mvwprintw(edit_window, LINES - 15, 2, "Mod size: %i",
            // char_buf->mod_size_);
            char_buf->mod_size_ = 0;
            wrefresh(edit_window);
            break;
    }
}
