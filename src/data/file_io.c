#include "../../ntutor.h"
#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <form.h>
#include <ncurses.h>
#include <panel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

DIR_ENTRY *initialize_dir_entry()
{
    DIR_ENTRY *d_entry = calloc(1, sizeof(DIR_ENTRY));
    if (!d_entry)
        return NULL;

    d_entry->name = (char *)malloc(50 * sizeof(char));
    d_entry->path = (char *)malloc(50 * sizeof(char));
    if (!d_entry->name || !d_entry->path)
    {
        free(d_entry->name);
        free(d_entry->path);
        free(d_entry);
        return NULL;
    }

    d_entry->state = 'c';
    d_entry->type = 'd';
    d_entry->num_of_open_entries = 0;
    d_entry->indent_level = 0;
    d_entry->last_in_sub_dir = false;
    d_entry->parent_dir = NULL;
    d_entry->prev = NULL;
    d_entry->next = NULL;

    return d_entry;
}

FILE_TREE *initialize_file_tree()
{
    FILE_TREE *f_tree = calloc(1, sizeof(FILE_TREE));

    f_tree->first_entry = NULL;
    f_tree->current_entry = NULL;
    f_tree->curr_entry_nr = 0;
    f_tree->num_of_entries = 0;

    return f_tree;
}

TEXT_BUFFER *initialize_buffer()
{
    TEXT_BUFFER *text_buf = malloc(sizeof(TEXT_BUFFER));

    text_buf->first_line = NULL;
    text_buf->current_line = text_buf->first_line;
    text_buf->num_of_lines = 0;
    text_buf->curr_line_nr = 0;
    text_buf->current_col = 0;
    text_buf->lines_to_print = 1;

    return text_buf;
}

I_TEXT_BUFFER *initialize_it_buffer()
{
    I_TEXT_BUFFER *text_buf = malloc(sizeof(I_TEXT_BUFFER));

    text_buf->first_line = NULL;
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
    line->length = 2;
    line->prev = NULL;
    line->next = NULL;

    return line;
}

I_LINE *initialize_iline()
{
    I_LINE *line = malloc(sizeof(I_LINE));

    line->buf_ = calloc(128, sizeof(char));
    line->buf_[0] = '\n';
    line->line_num = 0;
    line->length = 1;
    line->style = 0;
    line->centered = false;
    line->syntax_hl = false;
    line->prev = NULL;
    line->next = NULL;

    return line;
}

void deallocate_buffer(TEXT_BUFFER *tbuf)
{
    if (!tbuf)
        return;

    // FILE *log_file = fopen("t_buffer_dealloc_log.txt", "a");
    LINE *current_line = tbuf->first_line;
    // int i = 0;
    if (tbuf->num_of_lines > 1)
    {
        while (current_line)
        {

            // char buf[40];
            // memset(buf, 0, 40);
            // buf[39] = '\0';
            // snprintf(buf, 40, "%p\n", current_line);
            // fwrite(buf, 40, 1, log_file);

            free(current_line->buf_);
            LINE *next = current_line->next;
            free(current_line);
            current_line = next;
            // i++;
        }
    }
    else
    {
        if (tbuf->first_line)
        {
            if (tbuf->first_line->buf_)
                free(tbuf->first_line->buf_);
            free(tbuf->first_line);
        }
    }

    tbuf->num_of_lines = 0;
    tbuf->curr_line_nr = 0;
    tbuf->current_col = 0;

    // char buf[12];
    // memset(buf, 0, 12);
    // buf[11] = '\0';
    // snprintf(buf, 12, "%i\n", i);
    // fwrite(buf, 12, 1, log_file);

    free(tbuf); // <-- always free the buffer itself
}

void deallocate_it_buffer(I_TEXT_BUFFER *tbuf)
{
    if (!tbuf)
        return;

    I_LINE *current_line = tbuf->first_line;
    while (current_line)
    {
        free(current_line->buf_);
        I_LINE *next = current_line->next;
        free(current_line);
        current_line = next;
    }

    tbuf->num_of_lines = 0;
    tbuf->curr_line_nr = 0;
    tbuf->current_col = 0;

    free(tbuf);
}

void deallocate_file_tree(FILE_TREE *f_tree)
{
    DIR_ENTRY *curr_entry = f_tree->first_entry;

    // FILE *log_file = fopen("f_tree_dealloc_log.txt", "w");
    // int i = 0;
    while (curr_entry)
    {
        // char buf[40];
        // memset(buf, 0, 40);
        // buf[39] = '\0';
        // snprintf(buf, 40, "%p\n", curr_entry);
        // fwrite(buf, 40, 1, log_file);

        free(curr_entry->name);
        free(curr_entry->path);
        DIR_ENTRY *next = curr_entry->next;
        free(curr_entry);
        curr_entry = next;
        // i++;
    }
    // free(curr_entry->name);
    // free(curr_entry->path);
    free(curr_entry);
    // fclose(log_file);
    // free(f_tree->prev_dir->name);
    // free(f_tree->prev_dir->path);
    // free(f_tree->prev_dir);

    f_tree->first_entry = NULL;
    f_tree->current_entry = NULL;
    f_tree->num_of_entries = 0;
    f_tree->curr_entry_nr = 0;
    free(f_tree);
}

void prepare_empty_file(TEXT_BUFFER *tbuf)
{
    tbuf->first_line = initialize_line();
    tbuf->first_line->buf_[0] = ' ';
    tbuf->first_line->buf_[1] = '\n';
    tbuf->first_line->buf_[2] = '\0';
    tbuf->first_line->length = 3;
    tbuf->current_line = tbuf->first_line;
    tbuf->current_line->length = 3;
    tbuf->num_of_lines = 1;
}

void open_new_file(APP_CONTEXT *ctx)
{
    ctx->file = fopen(ctx->curr_file_path, "w+");

    if (ctx->file == NULL)
    {
        printf("Could not open %s.\n", ctx->curr_file_path);
    }

    if (ctx->file != NULL)
    {
        prepare_empty_file(ctx->t_buffer);
        // prepare_empty_file(ctx->t_buffer);
        print_file_metadata(ctx);

        ctx->t_buffer->lines_to_print = 1;

        print_buffer(ctx->t_buffer, &ctx->edit_window, &ctx->line_num_win,
                     &ctx->scroll_offset, ctx->t_buffer->lines_to_print);
        mvwprintw(ctx->edit_window, LINES - 7, EDIT_MAX - 15, "     ");
        mvwprintw(ctx->edit_window, LINES - 7, EDIT_MAX - 15, "0 : 0");

        rewind(ctx->file);

        print_buffer_label(ctx);
    }

    wnoutrefresh(ctx->line_num_win);
    wnoutrefresh(ctx->course_windows[2]);
    doupdate();
}

void open_file(APP_CONTEXT *ctx)
{
    ctx->file = fopen(ctx->file_tree->current_entry->path, "r+");
    strncpy(ctx->filename, ctx->file_tree->current_entry->name, 30);
    ctx->filename[29] = '\0';
    ctx->filename_len = strlen(ctx->filename);

    if (ctx->file == NULL)
    {
        printf("Could not open %s.\n", ctx->file_tree->current_entry->path);
    }

    if (ctx->file != NULL)
    {
        fseek(ctx->file, 0, SEEK_END);
        ctx->file_size = ftell(ctx->file);
        rewind(ctx->file);

        // mvwprintw(ctx->course_windows[3], LINES - 6, 3, "file_size: %i",
        // ctx->file_size); mvwprintw(ctx->course_windows[3], LINES - 7, 3,
        // "nol: %i", ctx->t_buffer->num_of_lines);

        if (ctx->file_size == 0)
        {
            prepare_empty_file(ctx->t_buffer);
        }
        else if (ctx->file_size > 0)
        {
            read_file_into_buffer(ctx->file, ctx->t_buffer);
        }

        // mvwprintw(ctx->course_windows[3], LINES - 6, 30, "file_size: %i",
        // ctx->file_size); mvwprintw(ctx->course_windows[3], LINES - 7, 30,
        // "nol: %i", ctx->t_buffer->num_of_lines);

        ctx->t_buffer->lines_to_print =
            ctx->t_buffer->num_of_lines > (ctx->editor_height - 4)
                ? (ctx->editor_height - 4)
                : ctx->t_buffer->num_of_lines;

        if (ctx->t_buffer->num_of_lines == 0)
            ctx->t_buffer->num_of_lines = 1;

        print_buffer(ctx->t_buffer, &ctx->edit_window, &ctx->line_num_win,
                     &ctx->scroll_offset, ctx->t_buffer->lines_to_print);

        rewind(ctx->file);

        print_buffer_label(ctx);
        print_file_metadata(ctx);
    }

    wnoutrefresh(ctx->course_windows[3]);
    doupdate();
}

void reprint_editor_buffer(APP_CONTEXT *ctx, bool activate_ed)
{
    // deallocate_buffer(ctx->t_buffer);
    // open_file(ctx);
    // mvwprintw(ctx->course_windows[3], LINES - 11, 30, "fsz: %i",
    // ctx->file_size); mvwprintw(ctx->course_windows[3], LINES - 10, 30, "nol:
    // %i", ctx->t_buffer->num_of_lines);
    if (ctx->file_size == 0 && ctx->t_buffer->num_of_lines == 0)
    {
        mvwprintw(ctx->course_windows[3], LINES - 9, 30, "%i",
                  ctx->t_buffer->num_of_lines);
        prepare_empty_file(ctx->t_buffer);
        ctx->t_buffer->lines_to_print = 1;
    }
    print_buffer_label(ctx);

    // mvwprintw(ctx->course_windows[3], LINES - 6, 3, "file_size: %i",
    // ctx->file_size); mvwprintw(ctx->course_windows[3], LINES - 7, 3, "nol:
    // %i", ctx->t_buffer->num_of_lines); mvwprintw(ctx->course_windows[3],
    // LINES - 8, 3, "%i", ctx->t_buffer->first_line->length);
    // wrefresh(ctx->course_windows[3]);

    print_buffer(ctx->t_buffer, &ctx->edit_window, &ctx->line_num_win,
                 &ctx->scroll_offset, ctx->t_buffer->lines_to_print);
    print_file_metadata(ctx);

    ctx->explorer_mode = false;
    // ctx->active_window_idx = ctx->editor_mode ? 2 : 0;

    // if (ctx->editor_mode)
    //     focus_window(&ctx->course_windows[0], ctx->editor_mode ? 2 : 3, "");

    unsigned short border_color = ctx->active_window_idx == 2 ? 3 : 2;

    focus_window(&ctx->course_windows[2], border_color, "Editor");
    if (activate_ed)
        curs_set(2);
}

void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf)
{
    char c;
    int i, j;
    i = j = 0;
    int line_number = 0;

    LINE *prev_line = NULL;
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
                // text_buf->first_line = initialize_line();
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
                if (j == 0)
                    prev_line = initialize_line();
                prev_line->next = curr_line;
                curr_line->prev = prev_line;
                prev_line = curr_line;
                curr_line = initialize_line();
            }
            j++;

            continue;
        }

        curr_line->buf_[i] = c;
        curr_line->length++;
        i++;
    }

    if (text_buf->num_of_lines == 0)
    {
        text_buf->num_of_lines = 1;
        curr_line->buf_[i] = '\n';
        text_buf->first_line = curr_line;
        text_buf->current_line = text_buf->first_line;
    }

    // free(prev_line->buf_);
    // free(prev_line);
    free(curr_line->buf_);
    free(curr_line);
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

void create_new_file(APP_CONTEXT *ctx, WINDOW **inner_win, WINDOW **form_window,
                     bool *new_file_form_active, FORM **new_file_form,
                     FIELD **field)
{
    create_new_file_input(inner_win, form_window, new_file_form, field,
                          "Create file");

    // hide_panel(ctx->explorer_panels[9]);
    top_panel(ctx->explorer_panels[1]);
    update_panels();
    doupdate();

    *new_file_form_active = true;

    while (*new_file_form_active)
    {
        keypad(*inner_win, TRUE);
        ctx->key = wgetch(*inner_win);

        switch (ctx->key)
        {
        case 263: // Backspace
            form_driver(*new_file_form, REQ_VALIDATION);
            FIELD *current = current_field(*new_file_form);
            char *buf = field_buffer(current, 0);
            trim(&buf);
            if (buf && strlen(buf) > 0)
            {
                form_driver(*new_file_form, REQ_DEL_PREV);
                wrefresh(*inner_win);
                wrefresh(*form_window);
            }
            break;
        case 10:
            form_driver(*new_file_form, REQ_VALIDATION);

            char *new_filename = field_buffer(field[0], 0);
            trim(&new_filename);
            int filename_len = strlen(new_filename);
            memset(ctx->filename, 0, filename_len);
            strncpy(ctx->filename, new_filename, strlen(new_filename));
            // mvwprintw(ctx->course_windows[3], LINES - 11, 20, "len: %lu",
            // strlen(new_filename));

            ctx->filename[filename_len] = '\0';
            ctx->filename_len = filename_len;

            if (ctx->file_tree->num_of_entries == 0)
            {
                strncpy(ctx->curr_file_path, field_buffer(field[0], 0), 30);
                ctx->curr_file_path[29] = '\0';
            }
            else if (ctx->file_tree->current_entry->state == 'o')
            {
                strcpy(ctx->curr_file_path,
                       ctx->file_tree->current_entry->path);
                strcat(ctx->curr_file_path, "/");
                strcat(ctx->curr_file_path, new_filename);
            }
            else if (ctx->file_tree->current_entry->parent_dir)
            {
                char *parent_path =
                    ctx->file_tree->current_entry->parent_dir->path;
                strncpy(ctx->curr_file_path, parent_path,
                        strlen(parent_path) + 1);
                memset(&ctx->curr_file_path[strlen(parent_path)], '/', 1);
                memmove(&ctx->curr_file_path[strlen(parent_path) + 1],
                        new_filename, strlen(new_filename));
                memset(&ctx->curr_file_path[strlen(parent_path) + 1 +
                                            strlen(new_filename)],
                       '\0', 1);
            }
            else
            {
                strncpy(ctx->curr_file_path, field_buffer(field[0], 0), 30);
                ctx->curr_file_path[29] = '\0';
            }
            // mvwprintw(ctx->course_windows[3], LINES - 9, 50, "new_filename3:
            // %s", new_filename);
            trim(&ctx->curr_file_path);
            *new_file_form_active = false;
            ctx->explorer_mode = false;
            ctx->editor_mode = true;
            ctx->active_window_idx = 2;

            deallocate_buffer(ctx->t_buffer);
            ctx->t_buffer = initialize_buffer();
            if (ctx->file && ctx->file->_fileno > 0)
                fclose(ctx->file);
            open_new_file(ctx);

            DIR_ENTRY *current_entry = NULL;
            if (ctx->file_tree->num_of_entries == 0)
            {
                ctx->file_tree->first_entry = initialize_dir_entry();
                current_entry = ctx->file_tree->first_entry;
                ctx->file_tree->current_entry = ctx->file_tree->first_entry;
            }
            else
            {
                current_entry = ctx->file_tree->current_entry;
            }
            // mvwprintw(ctx->course_windows[3], LINES - 7, 20, "new_filename:
            // %s", new_filename); mvwprintw(ctx->course_windows[3], LINES - 6,
            // 20, "ctx->filename: %s", ctx->filename);
            // mvwprintw(ctx->course_windows[3], LINES - 5, 20,
            // "ctx->filename_len: %i", ctx->filename_len);
            create_new_entry_for_file(ctx, current_entry, 8);

            unpost_form(*new_file_form);
            free_form(*new_file_form);
            free_field(field[0]);

            werase(ctx->course_windows[1]);
            create_file_tree(&ctx->course_windows[1], ctx->file_tree);
            focus_window(&ctx->course_windows[1], 2, "Explorer");
            focus_window(&ctx->course_windows[2], 3, "Editor");

            hide_panel(ctx->explorer_panels[1]);
            top_panel(ctx->explorer_panels[0]);
            update_panels();

            curs_set(2);
            wmove(ctx->edit_window, 0, 0);
            // wnoutrefresh(ctx->course_windows[1]);
            wnoutrefresh(ctx->line_num_win);
            wnoutrefresh(ctx->edit_window);
            doupdate();
            break;
        case KEY_F(1):
            *new_file_form_active = false;
            curs_set(0);
            unpost_form(*new_file_form);
            free_form(*new_file_form);
            free_field(field[0]);

            werase(ctx->course_windows[1]);
            create_file_tree(&ctx->course_windows[1], ctx->file_tree);
            focus_window(&ctx->course_windows[1], 3, "Explorer");

            hide_panel(ctx->explorer_panels[1]);
            top_panel(ctx->explorer_panels[0]);
            update_panels();
            doupdate();
            break;
        default:
            form_driver(*new_file_form, ctx->key);
            wrefresh(*form_window);
            break;
        }
    }
}

void create_new_entry_for_file(APP_CONTEXT *ctx, DIR_ENTRY *current_entry,
                               int type)
{
    DIR_ENTRY *new_entry = initialize_dir_entry();

    // special case if no existing entries in file tree yet
    if (ctx->file_tree->num_of_entries == 0)
    {
        strncpy(new_entry->name, ctx->filename, strlen(ctx->filename) + 1);
        strncpy(new_entry->path, ctx->filename, strlen(ctx->filename) + 1);
        new_entry->type = type;
        ctx->file_tree->first_entry = new_entry;
        ctx->file_tree->num_of_entries++;
        return;
    }

    // find correct position for new entry within file tree
    if (current_entry->state == 'o')
    {
        ctx->file_tree->current_entry = current_entry;
    }
    else
    {
        if (current_entry->parent_dir)
        {
            int parent_indent_level = current_entry->indent_level;

            ctx->file_tree->current_entry = current_entry->parent_dir->next;

            if (type == 4)
            {
                while (ctx->file_tree->current_entry->next &&
                       (ctx->file_tree->current_entry->type == 4 ||
                        ctx->file_tree->current_entry->indent_level >=
                            parent_indent_level + 1))
                {
                    ctx->file_tree->current_entry =
                        ctx->file_tree->current_entry->next;
                }
                ctx->file_tree->current_entry =
                    ctx->file_tree->current_entry->prev;
            }
            else if (type == 8)
            {
                if (current_entry != ctx->file_tree->first_entry)
                {
                    while (ctx->file_tree->current_entry->next &&
                           ((ctx->file_tree->current_entry->type != 8 ||
                             ctx->file_tree->current_entry->indent_level >=
                                 parent_indent_level + 1) ||
                            ctx->file_tree->current_entry->name[0] >
                                ctx->filename[0]))
                    {
                        ctx->file_tree->current_entry =
                            ctx->file_tree->current_entry->next;
                    }
                }
            }
        }
        else
        {
            ctx->file_tree->current_entry = ctx->file_tree->first_entry;

            if (type == 4)
            {
                while (ctx->file_tree->current_entry->next &&
                       (ctx->file_tree->current_entry->type == 4 ||
                        ctx->file_tree->current_entry->indent_level >
                            current_entry->indent_level))
                {
                    ctx->file_tree->current_entry =
                        ctx->file_tree->current_entry->next;
                }
                ctx->file_tree->current_entry =
                    ctx->file_tree->current_entry->prev;
            }
            else if (type == 8)
            {
                while (
                    ctx->file_tree->current_entry->next &&
                    ((ctx->file_tree->current_entry->type != 8 ||
                      ctx->file_tree->current_entry->indent_level >
                          current_entry->indent_level) ||
                     ctx->file_tree->current_entry->name[0] > ctx->filename[0]))
                {
                    ctx->file_tree->current_entry =
                        ctx->file_tree->current_entry->next;
                }
            }
        }
    }

    strncpy(new_entry->name, ctx->filename, strlen(ctx->filename) + 1);

    if (current_entry->state == 'o')
    {
        strcpy(new_entry->path, current_entry->path);
        strcat(new_entry->path, "/");
        strcat(new_entry->path, new_entry->name);

        if (current_entry->num_of_open_entries == 0)
        {
            new_entry->last_in_sub_dir = true;
        }

        DIR_ENTRY *entries_iterator = current_entry;

        while (entries_iterator)
        {
            entries_iterator->num_of_open_entries++;
            entries_iterator->num_of_entries++;
            entries_iterator = entries_iterator->parent_dir;
        }
        wrefresh(ctx->edit_window);

        new_entry->indent_level = current_entry->indent_level + 1;

        new_entry->parent_dir = current_entry;
    }
    else if (current_entry->parent_dir)
    {
        current_entry->parent_dir->num_of_open_entries++;
        current_entry->parent_dir->num_of_entries++;

        strcpy(new_entry->path, current_entry->parent_dir->path);
        memset(&new_entry->path[strlen(current_entry->parent_dir->path)], '/',
               1);
        memmove(&new_entry->path[strlen(current_entry->parent_dir->path) + 1],
                new_entry->name, strlen(new_entry->name));
        memset(&new_entry->path[strlen(current_entry->parent_dir->path) + 1 +
                                strlen(new_entry->name)],
               '\0', 1);
        new_entry->indent_level = current_entry->indent_level;
        new_entry->parent_dir = current_entry->parent_dir;

        if (current_entry->parent_dir->parent_dir)
        {
            current_entry->parent_dir->parent_dir->num_of_open_entries++;
            current_entry->parent_dir->parent_dir->num_of_entries++;
        }
    }
    else
    {
        strncpy(new_entry->path, ctx->filename, sizeof(ctx->filename) + 1);
        new_entry->path[sizeof(new_entry->path) + 1] = '\0';
        new_entry->indent_level = current_entry->indent_level;
        new_entry->parent_dir = current_entry->parent_dir;
    }

    new_entry->type = type;

    // link new entry into file tree linked list
    if (ctx->file_tree->num_of_entries > 1)
    {
        new_entry->prev = ctx->file_tree->current_entry;
        if (ctx->file_tree->current_entry->next)
        {
            new_entry->next = ctx->file_tree->current_entry->next;
            ctx->file_tree->current_entry->next->prev = new_entry;
        }
        ctx->file_tree->current_entry->next = new_entry;
    }
    else // when there is only one existing entry so far
    {
        if (current_entry->type == 4)
        {
            new_entry->prev = current_entry;
            current_entry->next = new_entry;
            ctx->file_tree->first_entry = current_entry;
            ctx->file_tree->current_entry = new_entry;
        }
        else if (current_entry->type == 8)
        {
            new_entry->next = current_entry;
            current_entry->prev = new_entry;
            ctx->file_tree->first_entry = new_entry;
            ctx->file_tree->current_entry = current_entry;
        }
    }

    ctx->file_tree->num_of_entries++;
}

void delete_file(APP_CONTEXT *ctx, bool *del_file_form_active,
                 WINDOW **inner_win, WINDOW **form_window, FORM **new_file_form,
                 FIELD **field)
{
    wrefresh(ctx->edit_window);
    if (ctx->file_tree->current_entry->type == 4 &&
        ctx->file_tree->current_entry->num_of_entries > 0)
    {
        WINDOW *cannot_delete_win =
            derwin(ctx->course_windows[1], 3, EXPLORER_WIDTH - 2,
                   ctx->file_tree->curr_entry_nr + 1, 1);
        draw_border(cannot_delete_win, 2, "");
        mvwprintw(cannot_delete_win, 1, 1, "Dir. not empty");
        wrefresh(cannot_delete_win);
        return;
    }

    char *filename = ctx->file_tree->current_entry->name;
    char *msg = "Delete ";
    char *label = malloc(strlen(msg) + strlen(filename) + 1);
    char *answer;
    strcpy(label, msg);
    strcat(label, filename);

    create_new_file_input(inner_win, form_window, new_file_form, field, label);

    top_panel(ctx->explorer_panels[3]);
    update_panels();
    doupdate();

    *del_file_form_active = true;

    while (*del_file_form_active)
    {
        keypad(*inner_win, TRUE);
        ctx->key = wgetch(*inner_win);

        switch (ctx->key)
        {
        case 263: // Backspace
            form_driver(*new_file_form, REQ_VALIDATION);
            FIELD *current = current_field(*new_file_form);
            char *buf = field_buffer(current, 0);
            trim(&buf);
            if (buf && strlen(buf) > 0)
            {
                form_driver(*new_file_form, REQ_DEL_PREV);
                wrefresh(*form_window);
            }
            break;
        case 10:
            form_driver(*new_file_form, REQ_VALIDATION);
            answer = field_buffer(field[0], 0);
            trim(&answer);

            if (strcmp(answer, "y") == 0)
            {
                *del_file_form_active = false;
                ctx->explorer_mode = true;
                ctx->editor_mode = false;
                ctx->active_window_idx = 1;

                if (strcmp(ctx->file_tree->current_entry->name,
                           ctx->filename) == 0)
                {
                    wclear(ctx->line_num_win);
                    // wclear(ctx->course_windows[2]);
                    wclear(ctx->edit_window);
                    mvwprintw(ctx->course_windows[2], 1, 5, "%s",
                              "                                     ");

                    print_no_open_file_msg(ctx);

                    deallocate_buffer(ctx->t_buffer);
                    ctx->t_buffer = initialize_buffer();
                }

                if (ctx->file && ctx->file->_fileno > 0)
                    fclose(ctx->file);

                remove(ctx->file_tree->current_entry->path);

                remove_entry_from_file_tree(ctx->file_tree);

                curs_set(0);
                unpost_form(*new_file_form);
                free_form(*new_file_form);
                free_field(field[0]);

                werase(ctx->course_windows[1]);
                create_file_tree(&ctx->course_windows[1], ctx->file_tree);
                focus_window(&ctx->course_windows[2], 2, "Editor");
                focus_window(&ctx->course_windows[1], 3, "Explorer");

                hide_panel(ctx->explorer_panels[3]);
                top_panel(ctx->explorer_panels[0]);
                update_panels();
                doupdate();

                print_no_open_file_msg(ctx);

                wnoutrefresh(ctx->course_windows[1]);
                wnoutrefresh(ctx->line_num_win);
                wnoutrefresh(ctx->course_windows[2]);
                wnoutrefresh(ctx->course_windows[3]);
                wnoutrefresh(ctx->edit_window);
                doupdate();
            }
            else
            {
                *del_file_form_active = false;
                curs_set(0);
                unpost_form(*new_file_form);
                free_form(*new_file_form);
                free_field(field[0]);

                werase(ctx->course_windows[1]);
                create_file_tree(&ctx->course_windows[1], ctx->file_tree);
                focus_window(&ctx->course_windows[1], 3, "Explorer");

                hide_panel(ctx->explorer_panels[3]);
                top_panel(ctx->explorer_panels[0]);
                update_panels();
                doupdate();
            }
            break;
        case KEY_F(1):
            *del_file_form_active = false;
            curs_set(0);
            unpost_form(*new_file_form);
            free_form(*new_file_form);
            free_field(field[0]);

            werase(ctx->course_windows[1]);
            create_file_tree(&ctx->course_windows[1], ctx->file_tree);
            focus_window(&ctx->course_windows[1], 3, "Explorer");

            hide_panel(ctx->explorer_panels[3]);
            top_panel(ctx->explorer_panels[0]);
            update_panels();
            doupdate();
            break;
        default:
            form_driver(*new_file_form, ctx->key);
            wrefresh(*form_window);
            break;
        }
    }

    free(label);
}

void remove_entry_from_file_tree(FILE_TREE *f_tree)
{
    if (f_tree == NULL || f_tree->current_entry == NULL)
    {
        return;
    }

    DIR_ENTRY *entry_to_remove = f_tree->current_entry;

    free(entry_to_remove->name);
    free(entry_to_remove->path);

    if (entry_to_remove->prev == NULL && entry_to_remove->next == NULL)
    {
        f_tree->current_entry = NULL; // List becomes empty
        f_tree->curr_entry_nr = 0;
    }
    else if (entry_to_remove->prev == NULL)
    {
        f_tree->current_entry = entry_to_remove->next;
        f_tree->current_entry->prev = NULL;
        f_tree->first_entry = f_tree->current_entry;
    }
    else if (entry_to_remove->next == NULL)
    {
        f_tree->current_entry = entry_to_remove->prev;
        f_tree->current_entry->next = NULL;
        f_tree->curr_entry_nr--;
    }
    else
    {
        f_tree->current_entry = entry_to_remove->prev; // Move to next node
        f_tree->curr_entry_nr--;
        entry_to_remove->prev->next = entry_to_remove->next;
        entry_to_remove->next->prev = entry_to_remove->prev;
    }

    if (entry_to_remove->parent_dir)
    {
        if (entry_to_remove->last_in_sub_dir &&
            entry_to_remove->parent_dir->num_of_open_entries > 1)
        {
            entry_to_remove->prev->last_in_sub_dir = true;
        }

        // entry_to_remove->parent_dir->num_of_entries--;

        DIR_ENTRY *entry_iterator = entry_to_remove;

        while (entry_iterator->parent_dir)
        {
            entry_iterator->parent_dir->num_of_open_entries--;
            entry_iterator->parent_dir->num_of_entries--;
            entry_iterator = entry_iterator->parent_dir;
        }

        // if (entry_to_remove->parent_dir->parent_dir)
        // {
        //     entry_to_remove->parent_dir->parent_dir->num_of_entries--;
        // }
    }

    f_tree->num_of_entries--;
    free(entry_to_remove);
}

void rename_file(APP_CONTEXT *ctx, WINDOW **inner_win, WINDOW **form_window,
                 FORM **new_file_form, FIELD **field)
{
    char *filename = ctx->file_tree->current_entry->name;
    char *label = "Rename file";
    bool rename_file_form_active = true;

    create_new_file_input(inner_win, form_window, new_file_form, field, label);
    set_field_buffer(field[0], 0, filename);
    form_driver(*new_file_form, REQ_END_LINE);

    wnoutrefresh(*form_window);
    top_panel(ctx->explorer_panels[2]);
    update_panels();
    doupdate();

    while (rename_file_form_active)
    {
        keypad(*inner_win, TRUE);
        ctx->key = wgetch(*inner_win);

        switch (ctx->key)
        {
        case 263: // Backspace
            form_driver(*new_file_form, REQ_VALIDATION);
            FIELD *current = current_field(*new_file_form);
            char *buf = field_buffer(current, 0);
            trim(&buf);
            if (buf && strlen(buf) > 0)
            {
                form_driver(*new_file_form, REQ_DEL_PREV);
                wrefresh(*form_window);
            }
            break;
        case 10:
            form_driver(*new_file_form, REQ_VALIDATION);
            char *new_filename = calloc(20, sizeof(char));
            new_filename = field_buffer(field[0], 0);
            trim(&new_filename);

            rename_file_form_active = false;
            ctx->explorer_mode = true;
            ctx->editor_mode = false;
            ctx->active_window_idx = 1;

            if (strcmp(ctx->file_tree->current_entry->name, new_filename) != 0)
            {
                char *new_path = NULL;
                if (ctx->file_tree->current_entry->parent_dir)
                {
                    new_path = malloc(
                        strlen(
                            ctx->file_tree->current_entry->parent_dir->path) +
                        strlen(new_filename) + 1);
                    strcpy(new_path,
                           ctx->file_tree->current_entry->parent_dir->path);
                    strcat(new_path, "/");
                    strcat(new_path, new_filename);
                }
                else
                {
                    new_path = malloc(sizeof(new_filename) + 1);
                    strcpy(new_path, new_filename);
                }

                rename(ctx->file_tree->current_entry->path, new_path);

                strcpy(ctx->file_tree->current_entry->path, new_path);
                strcpy(ctx->file_tree->current_entry->name, new_filename);
                strncpy(ctx->filename, ctx->file_tree->current_entry->name, 30);
                ctx->filename[29] = '\0';
                ctx->filename_len = strlen(ctx->filename);

                print_buffer_label(ctx);
                free(new_path);
            }

            curs_set(0);
            unpost_form(*new_file_form);
            free_form(*new_file_form);
            free_field(field[0]);

            wclear(ctx->course_windows[1]);
            create_file_tree(&ctx->course_windows[1], ctx->file_tree);

            focus_window(&ctx->course_windows[2], 2, "Editor");
            focus_window(&ctx->course_windows[1], 3, "Explorer");

            wnoutrefresh(ctx->course_windows[1]);
            wnoutrefresh(ctx->line_num_win);
            wnoutrefresh(ctx->edit_window);
            wnoutrefresh(ctx->course_windows[2]);
            doupdate();

            break;
        case KEY_F(1):
            rename_file_form_active = false;
            curs_set(0);
            unpost_form(*new_file_form);
            free_form(*new_file_form);
            free_field(field[0]);

            werase(ctx->course_windows[1]);
            create_file_tree(&ctx->course_windows[1], ctx->file_tree);
            focus_window(&ctx->course_windows[1], 3, "Explorer");

            hide_panel(ctx->explorer_panels[2]);
            top_panel(ctx->explorer_panels[0]);
            update_panels();
            doupdate();
            break;
        default:
            form_driver(*new_file_form, ctx->key);
            wrefresh(*form_window);
            break;
        }
    }
}

void create_directory(APP_CONTEXT *ctx, WINDOW **inner_win,
                      WINDOW **form_window, FORM **new_file_form, FIELD **field)
{
    bool make_dir_form_active = true;

    create_new_file_input(inner_win, form_window, new_file_form, field,
                          "Create directory");

    top_panel(ctx->explorer_panels[4]);
    update_panels();
    doupdate();

    while (make_dir_form_active)
    {
        keypad(*inner_win, TRUE);
        ctx->key = wgetch(*inner_win);

        switch (ctx->key)
        {
        case 263: // Backspace
            form_driver(*new_file_form, REQ_VALIDATION);
            FIELD *current = current_field(*new_file_form);
            char *buf = field_buffer(current, 0);
            trim(&buf);
            if (buf && strlen(buf) > 0)
            {
                form_driver(*new_file_form, REQ_DEL_PREV);
                wrefresh(*form_window);
            }
            break;
        case 10:
            form_driver(*new_file_form, REQ_VALIDATION);
            char *new_dirname = calloc(20, sizeof(char));
            new_dirname = field_buffer(field[0], 0);
            trim(&new_dirname);
            strcpy(ctx->filename, new_dirname);
            trim(&ctx->filename);

            make_dir_form_active = false;
            ctx->explorer_mode = true;
            ctx->editor_mode = false;
            ctx->active_window_idx = 1;

            char *new_path = NULL;
            if (ctx->file_tree->num_of_entries == 0)
            {
                new_path = malloc(strlen(new_dirname) + 1);
                strcpy(new_path, new_dirname);
            }
            else if (ctx->file_tree->current_entry->state == 'o')
            {
                new_path = malloc(strlen(ctx->file_tree->current_entry->path) +
                                  strlen(new_dirname) + 1);
                strcpy(new_path, ctx->file_tree->current_entry->path);
                strcat(new_path, "/");
                strcat(new_path, new_dirname);
            }
            else if (ctx->file_tree->current_entry->parent_dir)
            {
                new_path = malloc(
                    strlen(ctx->file_tree->current_entry->parent_dir->path) +
                    strlen(new_dirname) + 1);
                strcpy(new_path,
                       ctx->file_tree->current_entry->parent_dir->path);
                strcat(new_path, "/");
                strcat(new_path, new_dirname);
            }
            else
            {
                new_path = malloc(strlen(new_dirname) + 1);
                strcpy(new_path, new_dirname);
            }

            mkdir(new_path, 0777);
            free(new_path);

            DIR_ENTRY *current_entry = NULL;
            if (ctx->file_tree->num_of_entries == 0)
            {
                ctx->file_tree->first_entry = initialize_dir_entry();
                current_entry = ctx->file_tree->first_entry;
                ctx->file_tree->current_entry = ctx->file_tree->first_entry;
            }
            else
            {
                current_entry = ctx->file_tree->current_entry;
            }

            create_new_entry_for_file(ctx, current_entry, 4);

            curs_set(0);
            unpost_form(*new_file_form);
            free_form(*new_file_form);
            free_field(field[0]);

            wclear(ctx->course_windows[1]);
            create_file_tree(&ctx->course_windows[1], ctx->file_tree);

            focus_window(&ctx->course_windows[2], 2, "Editor");
            focus_window(&ctx->course_windows[1], 3, "Explorer");

            wnoutrefresh(ctx->course_windows[1]);
            wnoutrefresh(ctx->line_num_win);
            wnoutrefresh(ctx->edit_window);
            wnoutrefresh(ctx->course_windows[2]);
            doupdate();

            break;
        case KEY_F(1):
            make_dir_form_active = false;
            curs_set(0);
            unpost_form(*new_file_form);
            free_form(*new_file_form);
            free_field(field[0]);

            werase(ctx->course_windows[1]);
            create_file_tree(&ctx->course_windows[1], ctx->file_tree);
            focus_window(&ctx->course_windows[1], 3, "Explorer");

            hide_panel(ctx->explorer_panels[4]);
            top_panel(ctx->explorer_panels[0]);
            update_panels();
            doupdate();
            break;
        default:
            form_driver(*new_file_form, ctx->key);
            wrefresh(*form_window);
            break;
        }
    }
}

bool dir_exists(char *dir_name)
{
    struct stat s;
    int err = stat(dir_name, &s);

    if (-1 == err)
    {
        if (ENOENT == errno)
        {
            return false;
        }
        else
        {
            perror("stat");
        }
    }
    else
    {
        if (S_ISDIR(s.st_mode))
        {
            return true;
        }
    }

    return false;
}

void create_app_root_dir(APP_CONTEXT *ctx)
{
    char *home_dir = getenv("HOME");
    char docs_dir_buf[64];
    char ntutor_dir[64];
    char *proj_dir_buf = calloc(64, 1);

    snprintf(docs_dir_buf, strlen(home_dir) + 11, "%s/Documents", home_dir);

    if (dir_exists(docs_dir_buf))
    {
        snprintf(ntutor_dir, strlen(home_dir) + 18, "%s/Documents/ntutor",
                 home_dir);
        snprintf(proj_dir_buf,
                 strlen(home_dir) + 19 + strlen(ctx->user_data->name),
                 "%s/Documents/ntutor/%s", home_dir, ctx->user_data->name);
    }
    else
    {
        snprintf(ntutor_dir, strlen(home_dir) + 8, "%s/ntutor", home_dir);
        snprintf(proj_dir_buf,
                 strlen(home_dir) + 18 + strlen(ctx->user_data->name),
                 "%s/ntutor/%s", home_dir, ctx->user_data->name);
    }

    ctx->user_home_dir = proj_dir_buf;
    ctx->home_env = home_dir;

    set_user_home_dir(ctx);

    mkdir(ntutor_dir, 0777);
    mkdir(ctx->user_home_dir, 0777);
    chdir(ctx->user_home_dir);
}

void change_to_user_dir(APP_CONTEXT *ctx)
{
    if (dir_exists(ctx->user_data->home_dir))
    {
        chdir(ctx->user_data->home_dir);
    }
    else
    {
        create_app_root_dir(ctx);
    }
}
