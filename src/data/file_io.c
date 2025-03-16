#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define WU COLS / 12 // WU for WIDTH_UNIT
#define EDIT_MAX WU * 7 + 4

TEXT_BUFFER *initialize_buffer()
{
    TEXT_BUFFER *text_buf = malloc(sizeof(TEXT_BUFFER));

    text_buf->first_line = NULL;
    // text_buf->current_line = text_buf->first_line;
    text_buf->num_of_lines = 0;
    text_buf->curr_line_nr = 0;
    text_buf->current_col = 0;

    return text_buf;
}

LINE *initialize_line()
{
    LINE *line = malloc(sizeof(LINE));

    line->buf_ = calloc(100, sizeof(char));
    line->line_num = 0;
    line->length = 1;
    line->prev = NULL;
    line->next = NULL;

    return line;
}

void open_new_file(char *filename)
{
    FILE *file = fopen(filename, "w+");

    if (file == NULL)
    {
        printf("Could not open %s.\n", filename);
    }
}

FILE *open_file(const char *filename, TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                WINDOW **editor_window, WINDOW **edit_window,
                int *scroll_offset, int *lines_to_print)
{
    // DIR *dir = opendir(".");
    //
    // struct dirent *next = readdir(dir);
    //
    // bool file_exists = false;
    //
    // while ((next = readdir(dir)) != NULL)
    // {
    //     if (strcmp(next->d_name, filename) == 0)
    //     {
    //         file_exists = true;
    //         break;
    //     }
    // }
    //
    // closedir(dir);

    FILE *file = fopen(filename, "r+");

    if (file == NULL)
    {
        printf("Could not open %s.\n", filename);
    }

    if (file != NULL)
    {
        read_file_into_buffer(file, tbuf);

        *lines_to_print =
            tbuf->num_of_lines > LINES - 7 ? LINES - 7 : tbuf->num_of_lines;

        print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                     *lines_to_print);
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "     ");
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "0 : 0");

        rewind(file);

        wattron(*editor_window, A_BOLD | COLOR_PAIR(7));
        mvwprintw(*editor_window, 1, 4, "");
        wattroff(*editor_window, A_BOLD | COLOR_PAIR(7));
        wattron(*editor_window, A_BOLD | COLOR_PAIR(1));
        mvwprintw(*editor_window, 1, 6, "                                ");
        mvwprintw(*editor_window, 1, 6, "%s", filename);
        wattroff(*editor_window, A_BOLD | COLOR_PAIR(1));
        wrefresh(*edit_window);
    }

    wrefresh(*line_num_win);
    wrefresh(*editor_window);

    return file;
}

void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf)
{
    char c;
    int i, j;
    i = j = 0;
    unsigned short line_number = 0;

    LINE *prev_line = initialize_line();
    LINE *curr_line = initialize_line();

    while (fread(&c, sizeof(char), 1, file))
    {
        if (c == '\n')
        {
            curr_line->buf_[i] = c;
            curr_line->line_num = line_number;
            line_number++;
            i = 0;

            if (text_buf->num_of_lines == 0)
            {
                prev_line = curr_line;
                text_buf->first_line = prev_line;
                text_buf->current_line = prev_line;
                curr_line = initialize_line();
                text_buf->first_line->next = curr_line;
                curr_line->prev = text_buf->first_line;
                text_buf->num_of_lines++;
            }
            else
            {
                text_buf->num_of_lines++;
                prev_line->next = curr_line;
                curr_line->prev = prev_line;
                prev_line = curr_line;
                curr_line = initialize_line();
            }

            continue;
        }

        curr_line->buf_[i] = c;
        curr_line->length++;
        i++;
    }
}

void write_buffer_to_file(TEXT_BUFFER *tbuf, FILE *file, int y)
{
    tbuf->current_line = tbuf->first_line;
    while (tbuf->current_line != NULL)
    {
        fwrite(tbuf->current_line->buf_, sizeof(char),
               tbuf->current_line->length, file);
        tbuf->current_line = tbuf->current_line->next;
    }
    rewind(file);
    tbuf->current_line = tbuf->first_line;
    for (int i = 0; i < y; i++)
    {
        tbuf->current_line = tbuf->current_line->next;
    }
}

void deallocate_buffer(TEXT_BUFFER *tbuf)
{
    LINE *current_line = tbuf->first_line;
    while (NULL != current_line->next)
    {
        free(current_line->buf_);
        current_line = current_line->next;
        free(current_line->prev);
    }
    free(tbuf);
}
