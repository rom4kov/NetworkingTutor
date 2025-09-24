#include "../models/models.h"
#include <ncurses.h>

void log_ft_values(APP_CONTEXT *ctx)
{
    mvwprintw(ctx->course_windows[3], 6, 45, "                       ");
    mvwprintw(ctx->course_windows[3], 6, 45, "%i",
              ctx->file_tree->num_of_entries);
    mvwprintw(ctx->course_windows[3], 7, 45, "                       ");
    mvwprintw(ctx->course_windows[3], 7, 45, "%i",
              ctx->file_tree->current_entry->num_of_open_entries);
    wrefresh(ctx->course_windows[3]);
}

void log_editor_values(WINDOW **edit_window, int scroll_offset, TEXT_BUFFER *tbuf,
                int lines_to_print, int y, int x, int editor_height)
{
    mvwprintw(*edit_window, editor_height - 13, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 12, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 11, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 10, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 9, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 8, EDIT_MAX - 15, "        ");
    mvwprintw(*edit_window, editor_height - 13, EDIT_MAX - 15, "y:  %i", y);
    mvwprintw(*edit_window, editor_height - 12, EDIT_MAX - 15, "x:  %i", x);
    mvwprintw(*edit_window, editor_height - 11, EDIT_MAX - 15, "so:  %i",
              scroll_offset);
    mvwprintw(*edit_window, editor_height - 10, EDIT_MAX - 15, "nol: %i",
              tbuf->num_of_lines);
    mvwprintw(*edit_window, editor_height - 9, EDIT_MAX - 15, "ltp: %i",
              lines_to_print);
    mvwprintw(*edit_window, editor_height - 8, EDIT_MAX - 15, "cln: %i",
              tbuf->curr_line_nr);
}

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

