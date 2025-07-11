#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../course_tests/tests.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>

#define WINDOW_COUNT 4
#define WU COLS / 12 // WU for WIDTH_UNIT
#define EDIT_MAX WU * 7 + 4

void log_course_instr_values(APP_CONTEXT *ctx)
{
    mvwprintw(ctx->course_windows[2], 2, 3, "%s", "                    ");
    mvwprintw(ctx->course_windows[2], 3, 3, "%s", "                    ");
    mvwprintw(ctx->course_windows[2], 4, 3, "%s", "                    ");
    mvwprintw(ctx->course_windows[2], 5, 3, "%s", "                    ");
    mvwprintw(ctx->course_windows[2], 6, 3, "%s", "                    ");
    mvwprintw(ctx->course_windows[2], 2, 3, "curr_section %i",
              ctx->rp_state->curr_section);
    mvwprintw(ctx->course_windows[2], 3, 3, "nosi %i",
              ctx->rp_state->num_of_section_items[ctx->rp_state->curr_section]);
    mvwprintw(ctx->course_windows[2], 4, 3, "curr_item %i",
              ctx->rp_state->curr_item);
    mvwprintw(ctx->course_windows[2], 5, 3, "items_completed %i",
              ctx->rp_state->items_completed);
    mvwprintw(ctx->course_windows[2], 6, 3, "tsi cs %i",
              ctx->rp_state->total_section_items[ctx->rp_state->curr_section]);
}

void handle_course_input(APP_CONTEXT *ctx)
{
    if (ctx->active_window == 0)
    {
        switch (ctx->key)
        {
            case KEY_LEFT:
                menu_driver(ctx->start_menu, REQ_PREV_ITEM);
                wnoutrefresh(ctx->course_windows[0]);
                doupdate();
                break;
            case KEY_RIGHT:
                menu_driver(ctx->start_menu, REQ_NEXT_ITEM);
                wnoutrefresh(ctx->course_windows[0]);
                doupdate();
                break;
            case 9:
            case KEY_DOWN:
                ctx->active_window = 2;
                focus_window(&ctx->course_windows[0], 2, "Navigation");
                focus_window(&ctx->course_windows[2], 3, "Editor");
                doupdate();
                break;
            case '\n':
                ctx->curr_item = current_item(ctx->start_menu);
                if (item_index(ctx->curr_item) == 0)
                {
                    ctx->course_view_active = false;
                    ctx->start_view_active = true;
                    for (int i = 0; i < WINDOW_COUNT; ++i)
                    {
                        wclear(ctx->course_windows[i]);
                    }
                    ctx->course_view_active = false;
                    ctx->start_view_active = true;
                    ctx->start_needs_redraw = true;
                    break;
                }
                else if (item_index(ctx->curr_item) == 2)
                {
                    focus_window(&ctx->start_windows[0], 2, "Navigation");
                    focus_window(&ctx->start_windows[5], 3, "Details");
                    ctx->active_window = 5;
                    focus_window(&ctx->start_windows[5], 3, "Details");
                    wmove(ctx->start_windows[5], 4, 14);
                    wnoutrefresh(ctx->start_windows[5]);
                    doupdate();
                }
                break;
        }
    }
    else if (ctx->active_window == 1 && ctx->explorer_mode)
    {
        handle_explorer_input(ctx);
    }
    else if (ctx->active_window == 1)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_RIGHT:
                ctx->active_window = 2;
                focus_window(&ctx->course_windows[1], 2, "Explorer");
                focus_window(&ctx->course_windows[2], 3, "Editor");
                doupdate();
                break;
            case KEY_UP:
                ctx->active_window = 0;
                focus_window(&ctx->course_windows[1], 2, "Explorer");
                focus_window(&ctx->course_windows[0], 3, "Navigation");
                doupdate();
                break;
            case 10:
                ctx->explorer_mode = true;
                break;
        }
    }
    else if (ctx->active_window == 2 && ctx->editor_mode && ctx->file)
    {
        handle_editor_input(ctx);
    }
    else if (ctx->active_window == 2)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_LEFT:
                ctx->active_window = 1;
                focus_window(&ctx->course_windows[2], 2, "Editor");
                focus_window(&ctx->course_windows[1], 3, "Explorer");
                doupdate();
                break;
            case KEY_RIGHT:
                ctx->active_window = 3;
                focus_window(&ctx->course_windows[2], 2, "Editor");
                focus_instructions_window(ctx->rp_state, 3,
                                          "Course Instructions");
                wnoutrefresh(ctx->rp_state->inner_win);
                doupdate();
                break;
            case KEY_UP:
                ctx->active_window = 0;
                focus_window(&ctx->course_windows[0], 3, "Navigation");
                focus_window(&ctx->course_windows[2], 2, "Editor");
                doupdate();
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
    else if (ctx->active_window == 3)
    {
        switch (ctx->key)
        {
            case 9:
            case KEY_LEFT:
                ctx->active_window = 2;
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
                    set_items_completed(ctx);
                    print_next_course_item(ctx->rp_state);
                    // log_course_instr_values(ctx);

                    wnoutrefresh(ctx->rp_state->inner_win);
                    wnoutrefresh(ctx->course_windows[2]);
                    doupdate();
                }
                break;
            case 10:
                if (ctx->rp_state->curr_item ==
                        ctx->rp_state->num_of_section_items
                            [ctx->rp_state->curr_section] &&
                    ctx->rp_state->curr_section ==
                        ctx->rp_state->sections_completed)
                {
                    set_section_completed(ctx);
                    get_completed_sections(ctx);
                    ctx->rp_state->curr_item = 1;
                    // ctx->rp_state->items_completed = 0;
                    ctx->rp_state->curr_section += 1;
                    // set_items_completed(ctx);
                    set_items_completed(ctx);
                    ctx->rp_state->lines_to_print = 0;
                    ctx->rp_state->lines_excess = 0;
                    ctx->rp_state->scroll_offset = 0;
                    mvwprintw(ctx->course_windows[4], 1,
                              ctx->rp_state->window_width - 3, "%i",
                              ctx->rp_state->curr_section);
                    if (ctx->rp_state->curr_section > 0)
                    {
                        wclear(ctx->rp_state->inner_win);
                        char blank_line[ctx->rp_state->window_width - 4];
                        memset(blank_line, 32, sizeof(blank_line));
                        memset(&blank_line[sizeof(blank_line) - 1], '\0', 1);

                        mvwprintw(ctx->rp_state->right_panel, 9, 3, "%s",
                                  blank_line);
                        mvwprintw(ctx->rp_state->right_panel, 11, 3, "%s",
                                  blank_line);
                    }
                    deallocate_it_buffer(ctx->rp_state->it_buffer);
                    ctx->rp_state->it_buffer = initialize_it_buffer();
                    print_course_instructions(ctx);
                    log_course_instr_values(ctx);

                    wnoutrefresh(ctx->rp_state->right_panel);
                    wnoutrefresh(ctx->rp_state->inner_win);
                    wnoutrefresh(ctx->course_windows[4]);
                    doupdate();
                }
                break;
            case KEY_DOWN:
                if (ctx->rp_state->scroll_offset > 0)
                {
                    ctx->rp_state->scroll_offset--;
                    print_next_course_item(ctx->rp_state);
                    wnoutrefresh(ctx->rp_state->inner_win);
                    doupdate();
                }
                break;
            case KEY_UP:
                if (ctx->rp_state->lines_excess > ctx->rp_state->scroll_offset)
                {
                    ctx->rp_state->scroll_offset++;
                    print_next_course_item(ctx->rp_state);
                    wnoutrefresh(ctx->rp_state->inner_win);
                    doupdate();
                }
                break;
            case '<':
                if (ctx->rp_state->curr_section > 0)
                {
                    ctx->rp_state
                        ->num_of_section_items[ctx->rp_state->curr_section] =
                        ctx->rp_state->curr_item;
                    ctx->rp_state->curr_section--;
                    ctx->rp_state->curr_item =
                        ctx->rp_state
                            ->course_progress[ctx->rp_state->curr_section];
                    ctx->rp_state->scroll_offset = 0;
                    ctx->rp_state->lines_excess = 0;

                    wclear(ctx->rp_state->inner_win);
                    deallocate_it_buffer(ctx->rp_state->it_buffer);
                    ctx->rp_state->it_buffer = initialize_it_buffer();
                    // read_item_into_buffer(ctx);
                    print_course_instructions(ctx);
                    log_course_instr_values(ctx);

                    wnoutrefresh(ctx->course_windows[2]);
                    wnoutrefresh(ctx->rp_state->right_panel);
                    wnoutrefresh(ctx->rp_state->inner_win);
                    doupdate();
                }
                break;
            case '>':
                if (ctx->rp_state->curr_section <
                    ctx->rp_state->sections_completed)
                {
                    ctx->rp_state->curr_section++;
                    ctx->rp_state->curr_item =
                        ctx->rp_state
                            ->course_progress[ctx->rp_state->curr_section];
                    ctx->rp_state->scroll_offset = 0;
                    ctx->rp_state->lines_excess = 0;

                    wclear(ctx->rp_state->inner_win);

                    deallocate_it_buffer(ctx->rp_state->it_buffer);
                    ctx->rp_state->it_buffer = initialize_it_buffer();

                    print_course_instructions(ctx);
                    log_course_instr_values(ctx);

                    wnoutrefresh(ctx->course_windows[1]);
                    wnoutrefresh(ctx->course_windows[2]);
                    wnoutrefresh(ctx->edit_window);
                    wnoutrefresh(ctx->line_num_win);
                    wnoutrefresh(ctx->rp_state->right_panel);
                    wnoutrefresh(ctx->rp_state->inner_win);
                    doupdate();
                }
                break;
            case 't':
                if (ctx->rp_state->curr_item ==
                    ctx->rp_state
                        ->num_of_section_items[ctx->rp_state->curr_section] && 
                    ctx->rp_state->s_metadata->has_test)
                {
                    wclear(ctx->rp_state->right_panel);
                    wclear(ctx->rp_state->inner_win);
                    ctx->rp_state->curr_item = 0;
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
                    wrefresh(ctx->rp_state->right_panel);
                    wrefresh(ctx->rp_state->inner_win);
                }
                break;
            case 's':
                wclear(ctx->rp_state->inner_win);
                int trc = perform_tests(ctx);
                if (trc != 0)
                {
                    mvwprintw(ctx->course_windows[4], 1, 1, "%s",
                              "tests did not work");
                    wrefresh(ctx->course_windows[4]);
                }
                break;
        }
    }
}
