#include <curses.h>
#define _XOPEN_SOURCE 500

#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include "controllers.h"
#include <ncurses.h>
#include <unistd.h>

void handle_editor_input(APP_CONTEXT *ctx)
{
    getyx(ctx->edit_window, ctx->y, ctx->x);

    switch (ctx->key)
    {
        case KEY_RIGHT:
            move_right(ctx->t_buffer, &ctx->edit_window,
                       &ctx->course_windows[2], ctx->y, ctx->x,
                       ctx->editor_height);
            break;
        case KEY_LEFT:
            move_left(ctx->t_buffer, &ctx->edit_window, &ctx->course_windows[2],
                      ctx->y, ctx->x, ctx->editor_height);
            break;
        case KEY_DOWN:
            move_down(ctx->t_buffer, &ctx->line_num_win, &ctx->edit_window,
                      &ctx->course_windows[2], ctx->y, ctx->x,
                      &ctx->scroll_offset, ctx->t_buffer->lines_to_print,
                      ctx->editor_height);
            break;
        case KEY_UP:
            move_up(ctx->t_buffer, &ctx->line_num_win, &ctx->edit_window,
                    &ctx->course_windows[2], ctx->y, ctx->x,
                    &ctx->scroll_offset, &ctx->t_buffer->lines_to_print,
                    ctx->editor_height);
            break;
        case KEY_BACKSPACE:
            bs_delete_char_or_line(ctx->t_buffer, &ctx->line_num_win,
                                   &ctx->edit_window, &ctx->course_windows[2],
                                   ctx->y, ctx->x, &ctx->scroll_offset,
                                   &ctx->t_buffer->lines_to_print, ctx->editor_height,
                                   ctx->filename_len, &ctx->file_modified);
            break;
        case KEY_DC:
            delete_char_or_line(ctx->t_buffer, &ctx->line_num_win,
                                &ctx->edit_window, &ctx->course_windows[2],
                                ctx->y, ctx->x, &ctx->scroll_offset,
                                &ctx->t_buffer->lines_to_print, ctx->editor_height,
                                ctx->filename_len, &ctx->file_modified);
            break;
        case 9:
            insert_tab(ctx->t_buffer, &ctx->edit_window,
                       &ctx->course_windows[2], ctx->y, ctx->x,
                       &ctx->line_num_win, &ctx->t_buffer->lines_to_print,
                       &ctx->scroll_offset, ctx->editor_height,
                       ctx->filename_len, &ctx->file_modified);
            break;
        case 10:
            insert_line(ctx->t_buffer, &ctx->edit_window,
                        &ctx->course_windows[2], &ctx->line_num_win, ctx->y,
                        ctx->x, &ctx->scroll_offset, &ctx->t_buffer->lines_to_print,
                        ctx->editor_height, ctx->filename_len,
                        &ctx->file_modified);
            break;
        case KEY_HOME:
            move_to_start_of_line(ctx->t_buffer, &ctx->edit_window,
                                  &ctx->course_windows[2], ctx->y, ctx->x,
                                  ctx->editor_height);
            break;
        case KEY_END:
            move_to_end_of_line(ctx->t_buffer, &ctx->edit_window,
                                &ctx->course_windows[2], ctx->y, ctx->x,
                                ctx->editor_height);
            break;
        case KEY_F(1):
            curs_set(0);
            wrefresh(ctx->edit_window);
            ctx->editor_mode = false;
            break;
        case KEY_F(2):
            break;
        case KEY_F(10):
            write_buffer_to_file(ctx->t_buffer, ctx->file, ctx->y);
            ctx->file_modified = false;
            mvwprintw(ctx->course_windows[2], 1, 8 + ctx->filename_len, " ");
            print_file_metadata(ctx);
            wnoutrefresh(ctx->course_windows[2]);
            wmove(ctx->edit_window, ctx->y, ctx->x);
            wnoutrefresh(ctx->edit_window);
            doupdate();
            break;
        default:
            insert_char(ctx->t_buffer, &ctx->edit_window,
                        &ctx->course_windows[2], ctx->scroll_offset, ctx->y,
                        ctx->x, ctx->key, &ctx->line_num_win,
                        &ctx->t_buffer->lines_to_print, ctx->editor_height,
                        ctx->filename_len, &ctx->file_modified);
            break;
    }
}
