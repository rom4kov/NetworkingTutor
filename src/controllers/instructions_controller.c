#include "../core/core.h"
#include "../course_tests/tests.h"
#include "../models/models.h"
#include "../views/views.h"

#include <ncurses.h>


void handle_instr_window_input(APP_CONTEXT *ctx)
{

    switch (ctx->key)
    {
    case 9:
    case KEY_LEFT:
        ctx->active_window_idx = 2;
        ctx->active_window = ctx->course_windows[2];
        focus_window(&ctx->course_windows[2], 3, "Editor");
        focus_instructions_window(ctx->rp_state, 2, "Course Instructions");
        wnoutrefresh(ctx->rp_state->inner_win);
        doupdate();
        break;
    case ' ':
        if (ctx->rp_state->curr_item <
            ctx->rp_state->num_of_section_items[ctx->rp_state->curr_section])
        {
            ctx->rp_state->curr_item++;
            werase(ctx->rp_state->inner_win);
            set_items_completed(ctx);
            print_next_course_item(ctx->rp_state);
            // log_course_instr_values(ctx);

            if ((ctx->rp_state->curr_item ==
                 ctx->rp_state
                     ->num_of_section_items[ctx->rp_state->curr_section]) &&
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
                    ctx->rp_state
                        ->num_of_section_items[ctx->rp_state->curr_section] &&
                ctx->rp_state->curr_section ==
                    ctx->rp_state->sections_completed)
            {
                if (!ctx->rp_state->s_metadata->has_test)
                {
                    complete_section(ctx);
                }
                else if (ctx->run_sum && ctx->run_sum->nTestsFailed == 0)
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

            ctx->rp_state->num_of_section_items[ctx->rp_state->curr_section] =
                ctx->rp_state->curr_item;
            ctx->rp_state->curr_section--;

            get_course_progress(ctx);

            ctx->rp_state->curr_item =
                ctx->rp_state->course_progress[ctx->rp_state->curr_section] +
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
        if (ctx->rp_state->curr_section < ctx->rp_state->sections_completed)
        {
            free_section_data(ctx);

            if (ctx->rp_state->curr_section - 1 <
                ctx->rp_state->total_course_sections)
            {

                ctx->rp_state->showing_test_results = false;
                ctx->rp_state->curr_section++;
                get_course_progress(ctx);
                ctx->rp_state->curr_item =
                    ctx->rp_state->course_progress[ctx->rp_state->curr_section];
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
                ctx->rp_state
                    ->num_of_section_items[ctx->rp_state->curr_section] &&
            ctx->rp_state->s_metadata->has_test &&
            ctx->rp_state->s_metadata->has_separate_task)
        {
            wclear(ctx->rp_state->right_panel);
            wclear(ctx->rp_state->inner_win);

            ctx->rp_state->ready_to_test = true;

            get_task(ctx);
            wattron(ctx->rp_state->inner_win, A_BOLD | A_UNDERLINE);
            mvwprintw(ctx->rp_state->inner_win, 0, 0, "TASK FOR SECTION %i: %s",
                      ctx->rp_state->curr_section + 1,
                      ctx->rp_state->s_metadata->title);
            wattroff(ctx->rp_state->inner_win, A_BOLD | A_UNDERLINE);
            mvwprintw(ctx->rp_state->inner_win, 2, 0, "%s",
                      wrap_text(ctx->rp_state->current_task,
                                ctx->rp_state->window_width - 5));

            char *press_enter = "Press s to submit your task";
            mvwprintw(ctx->rp_state->right_panel, LINES - 5,
                      (ctx->rp_state->window_width - strlen(press_enter)) / 2,
                      "%s", press_enter);

            focus_window(&ctx->rp_state->right_panel, 3, "Course Instructions");
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
            ctx->rp_state->curr_section <= ctx->rp_state->sections_completed)
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
