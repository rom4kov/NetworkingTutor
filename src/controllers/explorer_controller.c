#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"

#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void log_ft_values(APP_CONTEXT *ctx)
{
    mvwprintw(ctx->course_windows[3], 6, 45, "                       ");
    mvwprintw(ctx->course_windows[3], 6, 45, "%i",
              ctx->file_tree->num_of_entries);
    mvwprintw(ctx->course_windows[3], 7, 45, "                       ");
    mvwprintw(ctx->course_windows[3], 7, 45, "%i",
              ctx->file_tree->current_entry->num_of_entries);
    wrefresh(ctx->course_windows[3]);
}

void handle_explorer_input(APP_CONTEXT *ctx)
{
    bool new_file_form_active = false;
    bool del_file_form_active = false;

    FORM *new_file_form = NULL;
    FIELD *field[2];

    switch (ctx->key)
    {
        case KEY_DOWN:
            if (ctx->file_tree->curr_entry_nr <
                ctx->file_tree->num_of_entries - 1)
            {
                move_to_next_entry(ctx->file_tree, &ctx->course_windows[1]);
                // log_ft_values(ctx);
            }
            break;
        case KEY_UP:
            if (ctx->file_tree->curr_entry_nr > 0)
            {
                move_to_prev_entry(ctx->file_tree, &ctx->course_windows[1]);
                // log_ft_values(ctx);
            }
            break;
        case 10:
            if (ctx->file_tree->current_entry->type == 4)
            {
                open_or_close_dir(ctx->file_tree, &ctx->course_windows[1]);
                // log_ft_values(ctx);
                break;
            }
            else
            {
                open_file_from_explorer(ctx, &new_file_form_active);
                // log_ft_values(ctx);
                break;
            }
            wrefresh(ctx->course_windows[1]);
            break;
        case 'a':
            create_new_file(ctx, &ctx->explorer_popup_windows[0],
                            &ctx->explorer_popup_windows[1],
                            &new_file_form_active, &new_file_form, field);
            // log_ft_values(ctx);
            break;
        case 'r':
            rename_file(ctx, &ctx->explorer_popup_windows[2],
                        &ctx->explorer_popup_windows[3], &new_file_form, field);
            break;
        case 'd':
            delete_file(ctx, &del_file_form_active,
                        &ctx->explorer_popup_windows[4],
                        &ctx->explorer_popup_windows[5], &new_file_form, field);
            // log_ft_values(ctx);
            break;
        case 'm':
            create_directory(ctx, &ctx->explorer_popup_windows[6],
                             &ctx->explorer_popup_windows[7], &new_file_form,
                             field);
            // log_ft_values(ctx);
            break;
        case '?':
            create_keybinds_window(&ctx->course_windows[1]);
            break;
        case KEY_F(1):
            wrefresh(ctx->course_windows[1]);
            ctx->explorer_mode = false;
            break;
    }
}
