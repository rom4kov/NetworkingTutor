#define _DEFAULT_SOURCE

#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <dirent.h>
#include <form.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WU COLS / 12 // WU for WIDTH_UNIT
#define EDIT_MAX WU * 7 + 4

TEXT_BUFFER *initialize_buffer()
{
    TEXT_BUFFER *text_buf = malloc(sizeof(TEXT_BUFFER));

    text_buf->first_line = calloc(100, sizeof(char));
    text_buf->current_line = text_buf->first_line;
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

void prepare_empty_file(TEXT_BUFFER **tbuf)
{
    (*tbuf)->first_line = initialize_line();
    (*tbuf)->first_line->buf_[0] = ' ';
    (*tbuf)->first_line->buf_[1] = '\n';
    (*tbuf)->first_line->length = 1;
    (*tbuf)->current_line = (*tbuf)->first_line;
    (*tbuf)->current_line->length = 1;
    (*tbuf)->num_of_lines = 1;
}

FILE *open_new_file(char *filename, TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                    WINDOW **editor_window, WINDOW **edit_window,
                    int *scroll_offset, int *lines_to_print)
{
    FILE *file = fopen(filename, "w+");

    if (file == NULL)
    {
        printf("Could not open %s.\n", filename);
    }

    if (file != NULL)
    {
        prepare_empty_file(&tbuf);

        *lines_to_print = 1;

        print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                     *lines_to_print);
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "     ");
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "0 : 0");

        rewind(file);

        mvwprintw(*editor_window, 1, 4, "                                ");
        ICON icon = print_file_icon((char *)filename);

        wattron(*editor_window, COLOR_PAIR(icon.color));
        mvwprintw(*editor_window, 1, 4, "%s", icon.icon);
        wattroff(*editor_window, COLOR_PAIR(icon.color));

        wattron(*editor_window, A_BOLD | COLOR_PAIR(1));
        mvwprintw(*editor_window, 1, 6, "%s", filename);
        wattroff(*editor_window, A_BOLD | COLOR_PAIR(1));
        wrefresh(*edit_window);
    }

    wrefresh(*line_num_win);
    wrefresh(*editor_window);

    return file;
}

FILE *open_file(const char *filename, TEXT_BUFFER *tbuf, WINDOW **line_num_win,
                WINDOW **editor_window, WINDOW **edit_window,
                int *scroll_offset, int *lines_to_print)
{
    int file_size = 0;
    FILE *file = fopen(filename, "r+");

    if (file == NULL)
    {
        printf("Could not open %s.\n", filename);
    }

    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);

        if (file_size == 0)
        {
            prepare_empty_file(&tbuf);
        }
        else
        {
            read_file_into_buffer(file, tbuf);
        }

        *lines_to_print =
            tbuf->num_of_lines > (LINES - 7) ? (LINES - 7) : tbuf->num_of_lines;

        print_buffer(tbuf, edit_window, line_num_win, scroll_offset,
                     *lines_to_print);
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "     ");
        mvwprintw(*edit_window, LINES - 7, EDIT_MAX - 15, "0 : 0");

        rewind(file);

        mvwprintw(*editor_window, 1, 4, "                                ");
        ICON icon = print_file_icon((char *)filename);

        wattron(*editor_window, COLOR_PAIR(icon.color));
        mvwprintw(*editor_window, 1, 4, "%s", icon.icon);
        wattroff(*editor_window, COLOR_PAIR(icon.color));

        wattron(*editor_window, A_BOLD | COLOR_PAIR(1));
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
    int file_size = 0;
    tbuf->current_line = tbuf->first_line;
    while (tbuf->current_line != NULL)
    {
        fwrite(tbuf->current_line->buf_, sizeof(char),
               tbuf->current_line->length, file);
        file_size += tbuf->current_line->length;
        tbuf->current_line = tbuf->current_line->next;
    }
    rewind(file);

    int fd = fileno(file);
    ftruncate(fd, file_size);

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

void create_new_file_input(WINDOW **inner_win, WINDOW **form_window,
                           FORM **new_file_form, FIELD **field, char *label)
{
    int rows, cols;

    field[0] = new_field(1, 14, 0, 0, 0, 0);
    field[1] = NULL;

    field_opts_off(field[0], O_AUTOSKIP);

    *new_file_form = new_form(field);

    scale_form(*new_file_form, &rows, &cols);

    set_form_win(*new_file_form, *inner_win);
    set_form_sub(*new_file_form, *form_window);

    draw_border(*inner_win, 1, "");
    mvwprintw(*inner_win, 0, 1, "%s", label);
    wrefresh(*inner_win);

    wclear(*form_window);
    wmove(*form_window, 0, 0);
    wrefresh(*form_window);
    curs_set(1);
    set_current_field(*new_file_form, field[0]);
    post_form(*new_file_form);
}
