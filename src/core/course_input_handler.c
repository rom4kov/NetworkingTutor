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
    mvwprintw(ctx->course_windows[2], 5, EDITOR_WIDTH - 30, "items_completed %i",
              ctx->rp_state->items_completed);
    mvwprintw(ctx->course_windows[2], 6, EDITOR_WIDTH - 30, "tsi cs %i",
              ctx->rp_state->total_section_items[ctx->rp_state->curr_section]);
    mvwprintw(ctx->course_windows[2], 7, EDITOR_WIDTH - 30,
              "total course sections %i", ctx->rp_state->total_course_sections);
    if (ctx->rp_state->s_metadata)
    {
        mvwprintw(ctx->course_windows[2], 8, EDITOR_WIDTH - 30, "has_test %i",
                  ctx->rp_state->s_metadata->has_test);
        mvwprintw(ctx->course_windows[2], 9, EDITOR_WIDTH - 30, "has sep task: %i",
                  ctx->rp_state->s_metadata->has_separate_task);
    }
    mvwprintw(ctx->course_windows[2], 10, EDITOR_WIDTH - 30, "test_mode: %i",
              ctx->rp_state->showing_test_results);
    mvwprintw(ctx->course_windows[2], 12, EDITOR_WIDTH - 30,
              "sections completed: %i", ctx->rp_state->sections_completed);
    mvwprintw(ctx->course_windows[2], 13, EDITOR_WIDTH - 30,
              "showing test results: %i", ctx->rp_state->showing_test_results);
    mvwprintw(ctx->course_windows[2], 14, EDITOR_WIDTH - 30,
              "course_progress: %i", ctx->rp_state->course_progress[ctx->rp_state->sections_completed - 1]);
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
                free(ctx->prev_dir);
                ctx->prev_dir = get_cwd();
                chdir(ctx->user_data->home_dir);
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
                focus_instructions_window(ctx->rp_state, 3,
                                          "Course Instructions");
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
                    wmove(ctx->edit_window, ctx->t_buffer->curr_line_nr,
                          ctx->t_buffer->current_col);
                else
                    wmove(ctx->edit_window, 0, 0);
                wrefresh(ctx->edit_window);
                break;
        }
    }
    else if (ctx->active_window_idx == 3)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_LEFT:
                ctx->active_window_idx = 2;
                ctx->active_window = ctx->course_windows[2];
                focus_window(&ctx->course_windows[2], 3, "Editor");
                focus_instructions_window(ctx->rp_state, 2,
                                          "Course Instructions");
                wnoutrefresh(ctx->rp_state->inner_win);
                doupdate();
                break;
            case ' ':
                if (ctx->rp_state->curr_item <
                    ctx->rp_state
                        ->num_of_section_items[ctx->rp_state->curr_section])
                {
                    ctx->rp_state->curr_item++;
                    werase(ctx->rp_state->inner_win);
                    set_items_completed(ctx);
                    print_next_course_item(ctx->rp_state);
                    // log_course_instr_values(ctx);

                    if ((ctx->rp_state->curr_item ==
                         ctx->rp_state->num_of_section_items
                             [ctx->rp_state->curr_section]) &&
                        ctx->rp_state->s_metadata->has_test == false)
                    {
                        ctx->rp_state->showing_test_results = false;
                        print_section_or_task_compl_msg(ctx, NULL);
                        wnoutrefresh(ctx->course_windows[4]);
                    }

                    if (ctx->rp_state->s_metadata->has_test &&
                        !ctx->rp_state->s_metadata->has_separate_task)
                    {
                        ctx->rp_state->showing_test_results = true;
                    }

                    wnoutrefresh(ctx->rp_state->inner_win);
                    wnoutrefresh(ctx->course_windows[2]);
                    doupdate();
                }
                break;
            case 10:
                // log_course_instr_values(ctx);
                // wrefresh(ctx->course_windows[2]);

                if (ctx->rp_state->curr_section + 1 <
                    ctx->rp_state->total_course_sections)
                {
                    if (ctx->rp_state->curr_item ==
                            ctx->rp_state->num_of_section_items
                                [ctx->rp_state->curr_section] &&
                        ctx->rp_state->curr_section ==
                            ctx->rp_state->sections_completed)
                    {
                        if (!ctx->rp_state->s_metadata->has_test)
                        {
                            complete_section(ctx);
                        }
                        else if (ctx->run_sum &&
                                 ctx->run_sum->nTestsFailed == 0)
                        {
                            ctx->run_sum = NULL;
                            complete_section(ctx);
                        }
                    }
                }
                else if (ctx->rp_state->showing_test_results)
                {
                    deallocate_it_buffer(ctx->rp_state->it_buffer);
                    ctx->rp_state->it_buffer = initialize_it_buffer();
                    complete_course(ctx);
                }
                break;
            case KEY_DOWN:
                if (ctx->rp_state->scroll_offset > 0)
                {
                    werase(ctx->rp_state->inner_win);
                    ctx->rp_state->scroll_offset--;
                    print_next_course_item(ctx->rp_state);
                    wnoutrefresh(ctx->rp_state->inner_win);
                    doupdate();
                }
                break;
            case KEY_UP:
                if (ctx->rp_state->lines_excess > ctx->rp_state->scroll_offset)
                {
                    werase(ctx->rp_state->inner_win);
                    ctx->rp_state->scroll_offset++;
                    print_next_course_item(ctx->rp_state);
                    wnoutrefresh(ctx->rp_state->inner_win);
                    doupdate();
                }
                break;
            case '<':
                if (ctx->rp_state->showing_test_results)
                {
                    ctx->rp_state->showing_test_results = false;
                    mvwprintw(ctx->rp_state->right_panel, LINES - 5, 2, "%s",
                              "<                ");
                    wclear(ctx->rp_state->inner_win);

                    // free_section_data(ctx);
                    deallocate_it_buffer(ctx->rp_state->it_buffer);

                    ctx->rp_state->it_buffer = initialize_it_buffer();
                    print_course_instructions(ctx);
                    ctx->rp_state->showing_test_results = false;
                    // log_course_instr_values(ctx);
                    // wnoutrefresh(ctx->course_windows[2]);

                    wnoutrefresh(ctx->rp_state->right_panel);
                    wnoutrefresh(ctx->rp_state->inner_win);
                    doupdate();
                }
                else if (ctx->rp_state->curr_section > 0 &&
                         ctx->rp_state->showing_test_results == false)
                {
                    if (!ctx->rp_state->showing_end_of_course_page)
                    {
                        free_section_data(ctx);
                    }

                    ctx->rp_state
                        ->num_of_section_items[ctx->rp_state->curr_section] =
                        ctx->rp_state->curr_item;
                    ctx->rp_state->curr_section--;

                    get_course_progress(ctx);

                    ctx->rp_state->curr_item =
                        ctx->rp_state
                            ->course_progress[ctx->rp_state->curr_section] +
                        (ctx->rp_state->curr_section == 0 ? 1 : 0);
                    ctx->rp_state->scroll_offset = 0;
                    ctx->rp_state->lines_excess = 0;

                    wclear(ctx->rp_state->inner_win);

                    deallocate_it_buffer(ctx->rp_state->it_buffer);
                    ctx->rp_state->it_buffer = initialize_it_buffer();

                    print_course_instructions(ctx);
                    // log_course_instr_values(ctx);

                    wnoutrefresh(ctx->course_windows[2]);
                    wnoutrefresh(ctx->rp_state->right_panel);
                    wnoutrefresh(ctx->rp_state->inner_win);
                    doupdate();
                }
                if (ctx->rp_state->showing_end_of_course_page)
                {
                    ctx->rp_state->showing_end_of_course_page = false;
                }
                // log_course_instr_values(ctx);
                // wrefresh(ctx->course_windows[2]);
                break;
            case '>':
                // log_course_instr_values(ctx);
                // wrefresh(ctx->course_windows[2]);
                if (ctx->rp_state->curr_section <
                    ctx->rp_state->sections_completed)
                {
                    free_section_data(ctx);

                    if (ctx->rp_state->curr_section - 1 <
                        ctx->rp_state->total_course_sections)
                    {

                        ctx->rp_state->showing_test_results = false;
                        ctx->rp_state->curr_section++;
                        get_course_progress(ctx);
                        ctx->rp_state->curr_item =
                            ctx->rp_state
                                ->course_progress[ctx->rp_state->curr_section];
                        ctx->rp_state->scroll_offset = 0;
                        ctx->rp_state->lines_excess = 0;

                        wclear(ctx->rp_state->inner_win);

                        deallocate_it_buffer(ctx->rp_state->it_buffer);
                        ctx->rp_state->it_buffer = initialize_it_buffer();

                        print_course_instructions(ctx);
                        // log_course_instr_values(ctx);

                        wnoutrefresh(ctx->rp_state->right_panel);
                        wnoutrefresh(ctx->rp_state->inner_win);
                        doupdate();
                    }
                    else
                    {
                        ctx->rp_state->curr_section++;
                        ctx->rp_state->showing_end_of_course_page = true;
                        deallocate_it_buffer(ctx->rp_state->it_buffer);
                        ctx->rp_state->it_buffer = initialize_it_buffer();
                        // print_course_complete(ctx);
                    }
                }
                // log_course_instr_values(ctx);
                // wrefresh(ctx->course_windows[2]);
                break;
            case 't':
                // log_course_instr_values(ctx);
                // wrefresh(ctx->course_windows[2]);
                if (ctx->rp_state->curr_item ==
                        ctx->rp_state->num_of_section_items
                            [ctx->rp_state->curr_section] &&
                    ctx->rp_state->s_metadata->has_test &&
                    ctx->rp_state->s_metadata->has_separate_task)
                {
                    wclear(ctx->rp_state->right_panel);
                    wclear(ctx->rp_state->inner_win);

                    ctx->rp_state->ready_to_test = true;

                    get_task(ctx);
                    wattron(ctx->rp_state->inner_win, A_BOLD | A_UNDERLINE);
                    mvwprintw(ctx->rp_state->inner_win, 0, 0,
                              "TASK FOR SECTION %i: %s",
                              ctx->rp_state->curr_section + 1,
                              ctx->rp_state->s_metadata->title);
                    wattroff(ctx->rp_state->inner_win, A_BOLD | A_UNDERLINE);
                    mvwprintw(ctx->rp_state->inner_win, 2, 0, "%s",
                              wrap_text(ctx->rp_state->current_task,
                                        ctx->rp_state->window_width - 5));

                    char *press_enter = "Press s to submit your task";
                    mvwprintw(
                        ctx->rp_state->right_panel, LINES - 5,
                        (ctx->rp_state->window_width - strlen(press_enter)) / 2,
                        "%s", press_enter);

                    focus_window(&ctx->rp_state->right_panel, 3,
                                 "Course Instructions");
                    wnoutrefresh(ctx->course_windows[2]);
                    wnoutrefresh(ctx->rp_state->right_panel);
                    wnoutrefresh(ctx->rp_state->inner_win);
                    doupdate();
                }
                break;
            case 's':
                // log_course_instr_values(ctx);
                // wrefresh(ctx->course_windows[2]);
                if (ctx->rp_state->ready_to_test &&
                    ctx->rp_state->curr_section <=
                        ctx->rp_state->sections_completed)
                {
                    wclear(ctx->rp_state->inner_win);
                    ctx->rp_state->showing_test_results = true;
                    mvwprintw(ctx->rp_state->right_panel, LINES - 5, 2, "%s",
                              "< Back to section");
                    wrefresh(ctx->rp_state->right_panel);
                    int trc = perform_tests(ctx);
                    
                    if (trc != 0)
                    {
                        mvwprintw(ctx->course_windows[4], 1, 1, "%s",
                                  "tests did not work");
                        wrefresh(ctx->course_windows[4]);
                    }
                    else
                    {
                        print_press_msg(ctx->rp_state);
                    }
                }
                break;
        }
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
