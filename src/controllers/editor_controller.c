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

    mvwprintw(edit_window, LINES - 16, 52, "After:");
    mvwprintw(edit_window, LINES - 15, 52, "Mod size: %i", cbuf->mod_size_);
    mvwprintw(edit_window, LINES - 14, 52, "File pointer: %li", ftell(file));
    mvwprintw(edit_window, LINES - 13, 52, "First byte of buffer: %p",
              cbuf->buf_);
    mvwprintw(edit_window, LINES - 12, 52, "ccur: %p", ccur);
    mvwprintw(edit_window, LINES - 11, 52, "cend: %p", cend);

    int first_buf_len = ccur - cbuf->buf_;
    mvwprintw(edit_window, LINES - 10, 52, "First buffer length: %i",
              first_buf_len);
    int second_buf_len = &cbuf->buf_[file_size] - cend;
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

void handle_editor_input(int ch, int *chars, WINDOW *edit_window, int y, int x,
                         CHAR_BUFFER *char_buf, LINE_BUFFER *line_buf,
                         int gap_size, bool *editor_mode, FILE *file,
                         int file_size, int *new_file_size)
{
    switch (ch)
    {
        case KEY_RIGHT:
            char_buf->ccur_++;
            char_buf->cend_++;
            char_buf->mod_size_++;
            getyx(edit_window, y, x);
            wmove(edit_window, y, x + 1);
            wrefresh(edit_window);
            break;
        case KEY_LEFT:
            char_buf->ccur_--;
            char_buf->cend_--;
            char_buf->mod_size_--;
            getyx(edit_window, y, x);
            wmove(edit_window, y, x - 1);
            wrefresh(edit_window);
            break;
        case KEY_DOWN:
            getyx(edit_window, y, x);
            char_buf->ccur_ += (line_buf->line_size_[line_buf->ccur_]);
            char_buf->cend_ += line_buf->line_size_[line_buf->ccur_];
            char_buf->mod_size_ += line_buf->line_size_[line_buf->ccur_];
            // if (line_buf->line_size_[line_buf->ccur_ + 1] >= x)
            // {
                wmove(edit_window, y + 1, x);
            // }
            // else
            // {
            //     wmove(edit_window, y + 1,
            //           line_buf->line_size_[line_buf->ccur_ + 1] - 2);
            // }
            wrefresh(edit_window);
            line_buf->ccur_++;
            break;
        case KEY_UP:
            char_buf->ccur_ -= line_buf->line_size_[line_buf->ccur_ - 1];
            char_buf->cend_ -= line_buf->line_size_[line_buf->ccur_ - 1];
            char_buf->mod_size_ -= line_buf->line_size_[line_buf->ccur_];
            line_buf->ccur_--;
            getyx(edit_window, y, x);
            wmove(edit_window, y - 1, x);
            wrefresh(edit_window);
            break;
        case KEY_BACKSPACE:
            if (char_buf->mod_size_ > 0)
                memcpy(char_buf->ccur_ - char_buf->mod_size_,
                       char_buf->cend_ - char_buf->mod_size_,
                       char_buf->mod_size_);
            else if (char_buf->mod_size_ < 0)
                memcpy(char_buf->cend_, char_buf->ccur_, char_buf->mod_size_);
            char_buf->mod_size_ = 0;
            char_buf->ccur_--;
            // gap_size++;
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
            *chars = line_buf->line_size_[line_buf->ccur_] - x;
            char *shift_str = malloc(*chars * sizeof(char));
            memcpy(shift_str, char_buf->cend_,
                   line_buf->line_size_[line_buf->ccur_] - x);
            mvwprintw(edit_window, y, x - 1, "%s ", shift_str);
            wmove(edit_window, y, x - 1);
            wrefresh(edit_window);
            free(shift_str);
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
            mvwprintw(edit_window, LINES - 9, 22, "test");
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
                memcpy(char_buf->cend_, char_buf->ccur_, char_buf->mod_size_);
            *char_buf->ccur_ = ch;
            char_buf->ccur_++;
            gap_size--;
            line_buf->line_size_[line_buf->ccur_] =
                line_buf->line_size_[line_buf->ccur_] + 1;
            *new_file_size += 1;
            getyx(edit_window, y, x);
            *chars = line_buf->line_size_[line_buf->ccur_] - x;
            char *insert_str = malloc(*chars * sizeof(char));
            memcpy(insert_str, char_buf->cend_,
                   line_buf->line_size_[line_buf->ccur_] - x);
            wprintw(edit_window, "%c%s", ch, insert_str);
            // wprintw(edit_window, "%c", ch);
            wmove(edit_window, y, x + 1);
            // mvwprintw(edit_window, LINES - 15, 2, "Mod size: %i",
            // char_buf->mod_size_);
            char_buf->mod_size_ = 0;
            wrefresh(edit_window);
            break;
    }
}

// void update_edit_window(CHAR_BUFFER *cbuf, LINE_BUFFER *lbuf,
//                         WINDOW **line_num_win, WINDOW **edit_window,
//                         int gap_size)
// {
//     int i, j, k;
//     i = j = k = 0;
//
//     for (i = 0; i < lbuf->size_; i++)
//     {
//         if (i < 9)
//             mvwprintw(*line_num_win, i + 1, 1, "%i", i + 1);
//         else
//             mvwprintw(*line_num_win, i + 1, 0, "%i", i + 1);
//         for (j = 0; j < lbuf->line_size_[i]; j++)
//         {
//             mvwprintw(*edit_window, i, j, "%c", cbuf->buf_[k + gap_size]);
//             k++;
//         }
//     }
// }
