#include <stdlib.h>
#define _DEFAULT_SOURCE

// #include <ctype.h>
#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"

#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void handle_explorer_input(APP_CONTEXT *ctx)
{
    bool new_file_form_active = false;
    bool del_file_form_active = false;
    WINDOW *inner_win = derwin(ctx->course_windows[1], 3, 18, 2, 2);
    WINDOW *form_window = derwin(inner_win, 1, 16, 1, 1);
    FORM *new_file_form = NULL;
    FIELD *field[2];

    switch (ctx->key)
    {
        case KEY_DOWN:
            if (ctx->file_tree->curr_entry_nr <
                ctx->file_tree->num_of_entries - 1)
            {
                move_to_next_entry(ctx->file_tree, &ctx->course_windows[1]);
            }
            break;
        case KEY_UP:
            if (ctx->file_tree->curr_entry_nr > 0)
            {
                move_to_prev_entry(ctx->file_tree, &ctx->course_windows[1]);
            }
            break;
        case 10:
            if (ctx->file_tree->current_entry->type == 4)
            {
                open_or_close_dir(ctx->file_tree, &ctx->course_windows[1]);
                break;
            }
            else
            {
                open_fiLe_from_explorer(ctx, &new_file_form_active);
                break;
            }
            wrefresh(ctx->course_windows[1]);
            break;
        case 'a':
            create_new_file(ctx, &form_window, &inner_win,
                            &new_file_form_active, &new_file_form, field);
            break;
        case 'r':
            break;
        case 'd':
            create_new_file_input(&inner_win, &form_window, &new_file_form,
                                  field, "Delete file");
            del_file_form_active = true;

            while (del_file_form_active)
            {
                ctx->key = getch();

                switch (ctx->key)
                {
                    case 263: // Backspace
                        form_driver(new_file_form, REQ_VALIDATION);
                        FIELD *current = current_field(new_file_form);
                        char *buf = field_buffer(current, 0);
                        trim(&buf);
                        if (buf && strlen(buf) > 0)
                        {
                            form_driver(new_file_form, REQ_DEL_PREV);
                            wrefresh(form_window);
                        }
                        break;
                    case 10:
                        form_driver(new_file_form, REQ_VALIDATION);
                        char *new_file_name = field_buffer(field[0], 0);
                        trim(&new_file_name);
                        del_file_form_active = false;
                        ctx->explorer_mode = true;
                        ctx->editor_mode = false;
                        ctx->active_window = 1;

                        if (strcmp(new_file_name, ctx->filename) == 0)
                        {
                            wclear(ctx->line_num_win);
                            wclear(ctx->course_windows[2]);
                            wclear(ctx->edit_window);
                            deallocate_buffer(ctx->t_buffer);
                            ctx->t_buffer = initialize_buffer();
                        }

                        if (ctx->file)
                            fclose(ctx->file);
                        remove(new_file_name);

                        ctx->course_windows[1] = create_explorer_window(
                            ctx->file_tree);

                        curs_set(0);
                        focus_window(&ctx->course_windows[2], 2, "Editor");
                        focus_window(&ctx->course_windows[1], 3, "Explorer");
                        wnoutrefresh(ctx->course_windows[1]);
                        wnoutrefresh(ctx->line_num_win);
                        wnoutrefresh(ctx->course_windows[2]);
                        wnoutrefresh(ctx->edit_window);
                        doupdate();
                        break;
                    case 'q':
                        del_file_form_active = false;
                        curs_set(0);
                        unpost_form(new_file_form);
                        free_form(new_file_form);
                        free_field(field[0]);
                        menu_driver(ctx->explorer_menu, REQ_NEXT_ITEM);
                        ctx->course_windows[1] = create_explorer_window(
                            ctx->file_tree);
                        focus_window(&ctx->course_windows[1], 3, "Explorer");
                        doupdate();
                        break;
                    default:
                        form_driver(new_file_form, ctx->key);
                        wrefresh(form_window);
                        break;
                }
            }
            break;
        case KEY_F(1):
            wrefresh(ctx->course_windows[1]);
            ctx->explorer_mode = false;
            break;
    }
}
