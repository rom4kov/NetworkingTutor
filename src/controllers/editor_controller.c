#define _XOPEN_SOURCE 500

#include "controllers.h"
#include "../core/core.h"
#include "../models/models.h"
#include "utils.h"
#include <ncurses.h>
#include <stdio.h>
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
    int second_buf_len =
        &cbuf->buf_[*new_file_size + cbuf->gap_size_] - cbuf->cend_;
    mvwprintw(edit_window, LINES - 16, 52, "After:");
    mvwprintw(edit_window, LINES - 15, 52, "Mod size: %i", cbuf->mod_size_);
    // mvwprintw(edit_window, LINES - 14, 52, "File pointer: %li", ftell(file));
    mvwprintw(edit_window, LINES - 13, 52, "First byte of buffer: %p",
              cbuf->buf_);
    mvwprintw(edit_window, LINES - 12, 52, "ccur: %p", ccur);
    mvwprintw(edit_window, LINES - 11, 52, "cend: %p", cend);

    mvwprintw(edit_window, LINES - 10, 52, "First buffer length: %i",
              first_buf_len);
    mvwprintw(edit_window, LINES - 9, 52, "Second buffer length: %i",
              second_buf_len);
    mvwprintw(edit_window, LINES - 8, 52, "Num of lines: %i", lbuf->size_);

    fwrite(cbuf->buf_, sizeof(char), first_buf_len, file);
    fwrite(cbuf->cend_, sizeof(char), second_buf_len, file);

    rewind(file);

    // int first_lbuf_len = lbuf->ccur_ - lbuf->new_lines_;
    // int second_lbuf_len =
    //     &lbuf->new_lines_[lbuf->size_ + lbuf->gap_size_] - lbuf->cend_;
    FILE *lines_file = fopen("../lines.txt", "r+");
    fwrite(lbuf->new_lines_, sizeof(int), lbuf->size_ + lbuf->gap_size_,
           lines_file);
    // fwrite(lbuf->cend_, sizeof(int), second_lbuf_len, lines_file);

    // int chars_diff = *new_file_size - file_size;
    mvwprintw(edit_window, LINES - 20, 52, "file_size: %i", file_size);
    mvwprintw(edit_window, LINES - 21, 52, "new_file_size: %i", *new_file_size);

    wrefresh(edit_window);
}

void handle_editor_input(int ch, WINDOW **edit_window,
                         // int y, int x,
                         TEXT_BUFFER *text_buf, bool *editor_mode
                         // , FILE *file
                         )
{
    int y, x;
    getyx(*edit_window, y, x);

    switch (ch)
    {
        case KEY_RIGHT:
            move_right(text_buf, edit_window, y, x);
            break;
        case KEY_LEFT:
            move_left(text_buf, edit_window, y, x);
            break;
        case KEY_DOWN:
            move_down(text_buf, edit_window, y, x);
            break;
        case KEY_UP:
            move_up(text_buf, edit_window, y, x);
            break;
        case KEY_BACKSPACE:
            break;
        case 10:
            break;
        case KEY_F(1):
            curs_set(0);
            wrefresh(*edit_window);
            *editor_mode = false;
            break;
        case KEY_F(10):
            // mvwprintw(*edit_window, LINES - 30, 52, "nfs before passing: %i",
            //           *new_file_size);
            // write_buffer_to_file(char_buf, file, file_size, new_file_size,
            //                      *edit_window, char_buf->ccur_, char_buf->cend_,
            //                      line_buf);
            break;
        default:
            break;
    }
}
