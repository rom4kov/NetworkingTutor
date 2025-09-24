#include "../../ntutor.h"
#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../course_tests/tests.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <panel.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void log_course_instr_values(APP_CONTEXT *ctx)
{
    mvwprintw(ctx->course_windows[2], 2, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 3, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 4, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 5, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 6, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 7, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 8, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 9, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 10, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 12, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 13, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 14, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 15, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 16, EDITOR_WIDTH - 30, "%s",
              "                    ");
    mvwprintw(ctx->course_windows[2], 2, EDITOR_WIDTH - 30, "curr_section %i",
              ctx->rp_state->curr_section);
    mvwprintw(ctx->course_windows[2], 3, EDITOR_WIDTH - 30, "nosi %i",
              ctx->rp_state->num_of_section_items[ctx->rp_state->curr_section]);
    mvwprintw(ctx->course_windows[2], 4, EDITOR_WIDTH - 30, "curr_item %i",
              ctx->rp_state->curr_item);
    mvwprintw(ctx->course_windows[2], 5, EDITOR_WIDTH - 30,
              "items_completed %i", ctx->rp_state->items_completed);
    mvwprintw(ctx->course_windows[2], 6, EDITOR_WIDTH - 30, "tsi cs %i",
              ctx->rp_state->total_section_items[ctx->rp_state->curr_section]);
    mvwprintw(ctx->course_windows[2], 7, EDITOR_WIDTH - 30,
              "total course sections %i", ctx->rp_state->total_course_sections);
    if (ctx->rp_state->s_metadata)
    {
        mvwprintw(ctx->course_windows[2], 8, EDITOR_WIDTH - 30, "has_test %i",
                  ctx->rp_state->s_metadata->has_test);
        mvwprintw(ctx->course_windows[2], 9, EDITOR_WIDTH - 30,
                  "has sep task: %i",
                  ctx->rp_state->s_metadata->has_separate_task);
    }
    mvwprintw(ctx->course_windows[2], 10, EDITOR_WIDTH - 30, "test_mode: %i",
              ctx->rp_state->showing_test_results);
    mvwprintw(ctx->course_windows[2], 12, EDITOR_WIDTH - 30,
              "sections completed: %i", ctx->rp_state->sections_completed);
    mvwprintw(ctx->course_windows[2], 13, EDITOR_WIDTH - 30,
              "showing test results: %i", ctx->rp_state->showing_test_results);
    mvwprintw(
        ctx->course_windows[2], 14, EDITOR_WIDTH - 30, "course_progress: %i",
        ctx->rp_state->course_progress[ctx->rp_state->sections_completed - 1]);
    mvwprintw(ctx->course_windows[2], 15, EDITOR_WIDTH - 30,
              "t_buffer->lines_to_print: %i", ctx->t_buffer->lines_to_print);
    mvwprintw(ctx->course_windows[2], 16, EDITOR_WIDTH - 30,
              "t_buf num_of_lines: %i", ctx->t_buffer->num_of_lines);
}

void handle_course_input(APP_CONTEXT *ctx)
{
    if (ctx->active_window_idx == 0)
    {
        handle_nav_input(ctx);
    }
    else if (ctx->active_window_idx == 1 && ctx->explorer_mode)
    {
        handle_explorer_input(ctx);
    }
    else if (ctx->active_window_idx == 1)
    {
        switch (ctx->key)
        {
        case 9:
        case KEY_RIGHT:
            ctx->active_window_idx = 2;
            ctx->active_window = ctx->course_windows[2];
            focus_window(&ctx->course_windows[1], 2, "Explorer");
            focus_window(&ctx->course_windows[2], 3, "Editor");
            doupdate();
            break;
        case KEY_UP:
            ctx->active_window_idx = 0;
            ctx->active_window = ctx->course_windows[0];
            focus_window(&ctx->course_windows[1], 2, "Explorer");
            focus_window(&ctx->course_windows[0], 3, "Navigation");
            doupdate();
            break;
        case 10:
            // free(ctx->prev_dir);
            // ctx->prev_dir = get_cwd();
            // chdir(ctx->user_data->home_dir);
            ctx->explorer_mode = true;
            break;
        }
    }
    else if (ctx->active_window_idx == 2 && ctx->editor_mode && ctx->file)
    {
        handle_editor_input(ctx);
    }
    else if (ctx->active_window_idx == 2)
    {
        switch (ctx->key)
        {
        case KEY_UP:
            ctx->active_window_idx = 0;
            ctx->active_window = ctx->course_windows[0];
            focus_window(&ctx->course_windows[2], 2, "Editor");
            focus_window(&ctx->course_windows[0], 3, "Navigation");
            doupdate();
            break;
        case KEY_DOWN:
            if (ctx->shell->terminal_active)
            {
                ctx->active_window_idx = SHELL_WINDOW_IDX;
                ctx->active_window = ctx->terminal_window;
                focus_window(&ctx->course_windows[2], 2, "Editor");
                focus_window(&ctx->terminal_window, 3, "Terminal");
                doupdate();
            }
            break;
        case 9:
        case KEY_LEFT:
            ctx->active_window_idx = 1;
            ctx->active_window = ctx->course_windows[1];
            focus_window(&ctx->course_windows[2], 2, "Editor");
            focus_window(&ctx->course_windows[1], 3, "Explorer");
            doupdate();
            break;
        case KEY_RIGHT:
            ctx->active_window_idx = 3;
            ctx->active_window = ctx->course_windows[3];
            focus_window(&ctx->course_windows[2], 2, "Editor");
            focus_instructions_window(ctx->rp_state, 3, "Course Instructions");
            wnoutrefresh(ctx->rp_state->inner_win);
            doupdate();
            break;
        case 't':
            if (!ctx->shell->terminal_active)
            {
                ctx->shell->terminal_active = true;
                ctx->shell->terminal_focused = true;
                ctx->active_window_idx = SHELL_WINDOW_IDX;
                recreate_editor_windows(ctx);

                ctx->active_window_idx = SHELL_WINDOW_IDX;
                toggle_terminal(ctx);

                print_term_buf(ctx->shell->term_inner_win,
                               ctx->shell->term_buffer);
                wmove(ctx->shell->term_inner_win, 0, 2);

                update_panels();
                curs_set(2);
                wnoutrefresh(ctx->course_windows[2]);
                wnoutrefresh(ctx->shell->term_inner_win);

                doupdate();
            }
            else
            {
                curs_set(0);
                ctx->shell->terminal_active = false;
                ctx->shell->terminal_focused = false;
                ctx->active_window_idx = 2;
                recreate_editor_windows(ctx);

                ctx->active_window_idx = 2;
                toggle_terminal(ctx);

                wmove(ctx->edit_window,
                      ctx->t_buffer->curr_line_nr - ctx->scroll_offset,
                      ctx->t_buffer->current_col);
                wnoutrefresh(ctx->edit_window);
                wnoutrefresh(ctx->terminal_window);
                wnoutrefresh(ctx->course_windows[2]);
                update_panels();
                doupdate();
            }
            break;
        case 10:
            ctx->editor_mode = true;
            if (ctx->file)
                curs_set(2);
            if (ctx->file)
            {
                if (ctx->y > ctx->editor_height - 5)
                {
                    ctx->t_buffer->current_line = ctx->t_buffer->first_line;
                    for (int i = 0; i < ctx->t_buffer->curr_line_nr; i++)
                    {
                        ctx->t_buffer->current_line =
                            ctx->t_buffer->current_line->next;
                    }
                }
                wmove(ctx->edit_window,
                      ctx->t_buffer->curr_line_nr - ctx->scroll_offset,
                      ctx->t_buffer->current_col);
            }
            else
                wmove(ctx->edit_window, 0, 0);
            wrefresh(ctx->edit_window);
            break;
        }
    }
    else if (ctx->active_window_idx == 3)
    {
        handle_instr_window_input(ctx);
    }
    else if (ctx->active_window_idx == SHELL_WINDOW_IDX &&
             ctx->shell->terminal_focused)
    {
        handle_terminal_input(ctx);
    }
    else if (ctx->active_window_idx == SHELL_WINDOW_IDX)
    {
        switch (ctx->key)
        {
        case KEY_UP:
            curs_set(0);
            ctx->active_window_idx = 2;
            ctx->active_window = ctx->course_windows[2];
            focus_window(&ctx->terminal_window, 2, "Terminal");
            focus_window(&ctx->course_windows[2], 3, "Editor");
            doupdate();
            break;
        case KEY_LEFT:
            curs_set(0);
            ctx->active_window_idx = 1;
            ctx->active_window = ctx->course_windows[1];
            focus_window(&ctx->terminal_window, 2, "Terminal");
            focus_window(&ctx->course_windows[1], 3, "Explorer");
            doupdate();
            break;
        case KEY_RIGHT:
            curs_set(0);
            ctx->active_window_idx = 3;
            ctx->active_window = ctx->course_windows[3];
            focus_window(&ctx->terminal_window, 2, "Terminal");
            focus_window(&ctx->course_windows[3], 3, "Course Details");
            doupdate();
            break;
        case '\n':
            ctx->shell->terminal_focused = true;

            chdir(ctx->shell_local_cwd);

            int nol = ctx->shell->term_buffer->num_of_lines;
            wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
                  ctx->shell->term_buffer->current_col);
            curs_set(2);
            wrefresh(ctx->shell->term_inner_win);
            break;
        case 't':
            curs_set(0);
            ctx->shell->terminal_active = false;
            ctx->active_window_idx = 2;
            ctx->editor_mode = false;
            recreate_editor_windows(ctx);
            toggle_terminal(ctx);
            wmove(ctx->edit_window,
                  ctx->t_buffer->curr_line_nr - ctx->scroll_offset,
                  ctx->t_buffer->current_col);
            wrefresh(ctx->edit_window);
            break;
        }
    }
}
