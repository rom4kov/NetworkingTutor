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


DIR_ENTRY *initialize_dir_entry(WINDOW **win, int num)
{
    DIR_ENTRY *d_entry = malloc(sizeof(DIR_ENTRY));

    d_entry->name = (char *)malloc(30 * sizeof(char));
    mvwprintw(*win, 30 + num, 2, "%p", &d_entry->name);
    // wrefresh(*win);
    d_entry->path = (char *)malloc(30 * sizeof(char));
    d_entry->state = 'c';
    d_entry->type = 'd';
    d_entry->num_of_entries = 0;
    d_entry->prev = NULL;
    d_entry->next = NULL;

    return d_entry;
}

FILE_TREE *initialize_file_tree()
{
    FILE_TREE *f_tree = calloc(1, sizeof(FILE_TREE));

    f_tree->first_entry = calloc(1, sizeof(DIR_ENTRY));
    f_tree->first_entry->name = (char *)malloc((30 * sizeof(char)));
    f_tree->first_entry->path = (char *)malloc((30 * sizeof(char)));
    f_tree->current_entry = f_tree->first_entry;
    f_tree->curr_entry_nr = 0;
    f_tree->num_of_entries = 0;

    return f_tree;
}

TEXT_BUFFER *initialize_buffer()
{
    TEXT_BUFFER *text_buf = malloc(sizeof(TEXT_BUFFER));

    text_buf->first_line = initialize_line();
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

void open_new_file(APP_CONTEXT *ctx)
{
    ctx->file = fopen(ctx->filename, "w+");

    if (ctx->file == NULL)
    {
        printf("Could not open %s.\n", ctx->filename);
    }

    if (ctx->file != NULL)
    {
        prepare_empty_file(&ctx->t_buffer);

        ctx->lines_to_print = 1;

        print_buffer(ctx->t_buffer, &ctx->edit_window, &ctx->line_num_win, &ctx->scroll_offset,
                     ctx->lines_to_print);
        mvwprintw(ctx->edit_window, LINES - 7, EDIT_MAX - 15, "     ");
        mvwprintw(ctx->edit_window, LINES - 7, EDIT_MAX - 15, "0 : 0");

        rewind(ctx->file);

        mvwprintw(ctx->course_windows[2], 1, 4, "                                ");
        ICON icon = print_file_icon((char *)ctx->filename);

        wattron(ctx->course_windows[2], COLOR_PAIR(icon.color));
        mvwprintw(ctx->course_windows[2], 1, 4, "%s", icon.icon);
        wattroff(ctx->course_windows[2], COLOR_PAIR(icon.color));

        wattron(ctx->course_windows[2], A_BOLD | COLOR_PAIR(1));
        mvwprintw(ctx->course_windows[2], 1, 6, "%s", ctx->filename);
        wattroff(ctx->course_windows[2], A_BOLD | COLOR_PAIR(1));
        wrefresh(ctx->edit_window);
    }

    wrefresh(ctx->line_num_win);
    wrefresh(ctx->course_windows[2]);
}

void open_file(APP_CONTEXT *ctx)
{
    int file_size = 0;
    ctx->file = fopen(ctx->filename, "r+");

    if (ctx->file == NULL)
    {
        printf("Could not open %s.\n", ctx->filename);
    }

    if (ctx->file != NULL)
    {
        fseek(ctx->file, 0, SEEK_END);
        file_size = ftell(ctx->file);
        rewind(ctx->file);

        if (file_size == 0)
        {
            prepare_empty_file(&ctx->t_buffer);
        }
        else if (ctx->t_buffer->first_line->length < 2)
        {
            read_file_into_buffer(ctx->file, ctx->t_buffer);
        }

        ctx->lines_to_print = ctx->t_buffer->num_of_lines > (LINES - 7)
                                  ? (LINES - 7)
                                  : ctx->t_buffer->num_of_lines;

        print_buffer(ctx->t_buffer, &ctx->edit_window, &ctx->line_num_win, &ctx->scroll_offset,
                     ctx->lines_to_print);
        mvwprintw(ctx->edit_window, LINES - 7, EDIT_MAX - 15, "     ");
        mvwprintw(ctx->edit_window, LINES - 7, EDIT_MAX - 15, "0 : 0");

        rewind(ctx->file);

        mvwprintw(ctx->course_windows[2], 1, 4, "                                ");
        ICON icon = print_file_icon((char *)ctx->filename);

        wattron(ctx->course_windows[2], COLOR_PAIR(icon.color));
        mvwprintw(ctx->course_windows[2], 1, 4, "%s", icon.icon);
        wattroff(ctx->course_windows[2], COLOR_PAIR(icon.color));

        wattron(ctx->course_windows[2], A_BOLD | COLOR_PAIR(1));
        mvwprintw(ctx->course_windows[2], 1, 6, "%s", ctx->filename);
        wattroff(ctx->course_windows[2], A_BOLD | COLOR_PAIR(1));
        wrefresh(ctx->edit_window);
    }

    wrefresh(ctx->line_num_win);
    wrefresh(ctx->course_windows[2]);
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
    if (tbuf->first_line == NULL)
        return;

    LINE *current_line = tbuf->first_line;
    while (current_line->next != NULL)
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
