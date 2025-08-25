#define _XOPEN_SOURCE_EXTENDED 1

#include "../../core/core.h"
#include "../../models/models.h"
#include "../views.h"
#include "start_menu.h"
#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_COUNT 6
#define WU COLS / 12 // WU for WIDTH_UNIT
#define WIDTH_REMAINDER COLS % WU
#define CARD_WIDTH (((WU * 7) / 3) + 1)
#define REMAINDER (WU * 7 + 4) % CARD_WIDTH

char *PROGRAMM_DESC =
    "Welcome to NetworkingTutor!\n\n"
    "NetworkingTutor is an interactive learning tool that aims to teach you "
    "the basics of networking.\n"
    "Start by creating a simple HTTP server while learning about "
    "sockets, HTTP, and low-level\n"
    "networking concepts. Each section presents "
    "challenges, a built-in text editor, and instant\n"
    "feedback to reinforce your learning. "
    "Gamification keeps you engaged as you progress.\n"
    "Expand your knowledge step by step and master "
    "networking fundamentals—one challenge at a time.\nReady to begin? 🚀";

void create_start_screen(APP_CONTEXT *ctx)
{
    ctx->start_windows[0] =
        create_navigation_window(&ctx->active_window_idx, &ctx->start_menu);
    ctx->start_windows[1] = create_header_section(ctx);
    ctx->start_windows[2] =
        create_course_preview_card(ctx, 0, 2, &ctx->courses[0]);
    ctx->start_windows[3] =
        create_course_preview_card(ctx, CARD_WIDTH, 3, &ctx->courses[1]);
    ctx->start_windows[4] =
        create_course_preview_card(ctx, CARD_WIDTH * 2, 4, &ctx->courses[2]);
    ctx->start_windows[5] = create_right_side_panel(ctx, " Details ");

    wrefresh(ctx->start_windows[5]);
}

WINDOW *create_navigation_window(int *active_win, MENU **start_menu)
{
    WINDOW *navigation;
    navigation = newwin(3, WU * 7 + 4, 0, 0);
    draw_border(navigation, *active_win == 0 ? 3 : 2, "");
    *start_menu = create_start_menu(navigation);

    wattron(navigation, COLOR_PAIR(3));
    mvwprintw(navigation, 0, 2, " Navigation ");
    wattroff(navigation, COLOR_PAIR(3));
    wrefresh(navigation);
    return navigation;
}

WINDOW *create_header_section(APP_CONTEXT *ctx)
{
    int header_height = LINES / 2 + 1;
    int header_width = WU * 7 + 4;
    WINDOW *header_window = newwin(header_height, header_width, 3, 0);
    WINDOW *header_inner =
        derwin(header_window, header_height - 2, header_width - 2, 1, 1);
    if (ctx->active_window_idx == 1)
    {
        wattron(header_inner, COLOR_PAIR(1) | A_BOLD);
    }

    I_TEXT_BUFFER *header_tbuf = initialize_it_buffer();
    read_window_text_into_buffer(ctx, header_tbuf, header_width, 0, 0, PROGRAMM_DESC);

    wattron(header_inner, A_BOLD);
    print_window_content(header_tbuf, header_inner, header_width - 2);
    wattroff(header_inner, A_BOLD);
    // mvwprintw(header_inner, 13, 0, "%s", PROGRAMM_DESC);
    if (ctx->active_window_idx == 1)
    {
        wattroff(header_inner, COLOR_PAIR(1));
    }

    mvwprintw(header_inner, 14,
              (header_width - (strlen((char *)ctx->user_data->name) + 10)) / 2,
              "Hello %s!", ctx->user_data->name);

    draw_border(header_window, 2, "Header");
    wrefresh(header_window);
    wrefresh(header_inner);
    return header_window;
}

WINDOW *create_course_preview_card(APP_CONTEXT *ctx, int x_position,
                                   int curr_win_idx, COURSE *course)
{
    int remainder = 0;
    if (ctx->start_view_active && curr_win_idx == 4)
    {
        remainder = REMAINDER;
    }

    int height = 0;
    int width = 0;
    int y_position = 0;
    int x_add = 0;
    if (ctx->start_view_active)
    {
        y_position = LINES / 2 + 4;
        height = LINES % 2 == 0 ? LINES / 2 - 4 : LINES / 2 - 3;
        width = CARD_WIDTH;
    }
    else if (ctx->progress_view_active || ctx->all_courses_view_active)
    {
        height = LINES / 3 + 2;
        y_position = course->id < 6 ? 12 : (13 + height);
        x_add = ctx->progress_view_active ? 80 : 0;
        width = CARD_WIDTH - (ctx->progress_view_active ? 2 : 4);
    }
    WINDOW *course_preview_card_outer =
        newwin(height, width + remainder, y_position, x_position + x_add);
    WINDOW *course_preview_card_inner = derwin(
        course_preview_card_outer, height - 3, width - 2 + remainder, 2, 1);

    if ((ctx->active_window_idx == 2 || ctx->active_window_idx == 3 ||
         ctx->active_window_idx == 4) &&
        curr_win_idx == ctx->active_window_idx && ctx->start_view_active)
    {
        draw_border(course_preview_card_outer, 3, " Course");
    }
    else
    {
        draw_border(course_preview_card_outer, 2, " Course");
        mvwaddch(course_preview_card_outer, 2, 0, ACS_LTEE);
        mvwaddch(course_preview_card_outer, 2, width - 1 + remainder, ACS_RTEE);
    }

    mvwhline(course_preview_card_outer, 2, 1, ACS_HLINE, width - 2 + remainder);
    wattron(course_preview_card_outer, COLOR_PAIR(3));
    mvwprintw(course_preview_card_outer, 0, 2, " Course #%i ", course->id);
    wattroff(course_preview_card_outer, COLOR_PAIR(3));

    mvwprintw(course_preview_card_outer, 1,
              (width - strlen(course->name)) / 2 - 1, "%s", course->name);

    ctx->card_buffers[curr_win_idx - 2] = initialize_it_buffer();
    read_window_text_into_buffer(
        ctx, ctx->card_buffers[curr_win_idx - 2], width - 2 + remainder, 1,
        curr_win_idx - 2, "");

    print_window_content(ctx->card_buffers[curr_win_idx - 2],
                         course_preview_card_inner, width - 2 + remainder);

    if (ctx->progress_view_active)
    {
        int start_x = 14;
        int comp_percent = get_course_completion_percentage(ctx, course->id);

        wattron(course_preview_card_outer, COLOR_PAIR(4));
        mvwprintw(course_preview_card_outer, height - 1, start_x, " %i%% complete ",
                  comp_percent);
        wattroff(course_preview_card_outer, COLOR_PAIR(4));
    }

    wnoutrefresh(course_preview_card_outer);
    wnoutrefresh(course_preview_card_inner);
    doupdate();
    return course_preview_card_outer;
}

WINDOW *create_right_side_panel(APP_CONTEXT *ctx, char *label)
{
    init_right_panel_state(ctx->rp_state, ctx->course_view_active);

    if (ctx->active_window_idx == 5)
    {
        draw_border(ctx->rp_state->right_panel, 3, "Right Panel");
    }
    else
    {
        draw_border(ctx->rp_state->right_panel, 2, "Right Panel");
    }

    wattron(ctx->rp_state->right_panel, COLOR_PAIR(3));
    mvwprintw(ctx->rp_state->right_panel, 0, 2, "%s", label);
    wattroff(ctx->rp_state->right_panel, COLOR_PAIR(3));

    if (ctx->start_view_active)
    {
        // ctx->user_data = get_user_data(ctx->db, ctx->current_user_id);
        // mvwprintw(ctx->rp_state->right_panel, 2, 3, "Your name: %s",
        //           ctx->user_data->name);
        // mvwprintw(ctx->rp_state->right_panel, 3, 3, "Created at: %s",
        //           ctx->user_data->created_at);
        // mvwprintw(ctx->rp_state->right_panel, 4, 3, "%i", ctx->current_user_id);
        print_intro(ctx);
        wnoutrefresh(ctx->rp_state->inner_win);
    }
    else if (ctx->course_view_active)
    {
        print_course_instructions(ctx);
        wnoutrefresh(ctx->rp_state->inner_win);
    }

    wnoutrefresh(ctx->rp_state->right_panel);
    doupdate();
    return ctx->rp_state->right_panel;
}

MENU *create_start_menu(WINDOW *nav_window)
{
    const char *choices[] = {
        "Home",      "All courses", "Account", "Progress", "Settings", "Shortcuts",
        (char *)NULL // Last element must be NULL
    };

    ITEM **menu_items = (ITEM **)calloc(7, sizeof(ITEM *));

    for (int i = 0; choices[i] != NULL; i++)
    {
        menu_items[i] = new_item(choices[i], "");
    }

    TABSIZE = 12;

    int MENU_SPACING;

    if (COLS > 240)
    {
        MENU_SPACING = 12;
    }
    else if (COLS > 220)
    {
        MENU_SPACING = 10;
    }
    else if (COLS > 200)
    {
        MENU_SPACING = 8;
    }
    else if (COLS > 180)
    {
        MENU_SPACING = 6;
    }
    else
    {
        MENU_SPACING = 4;
    }

    // Create the menu
    MENU *menu = new_menu(menu_items);
    set_menu_format(menu, 1, 12);
    set_menu_spacing(menu, 0, 1, MENU_SPACING);
    // mvwprintw(nav_window, 2, 3, "%i", COLS);

    // Set the window for the menu to be displayed inside left_inner_win
    set_menu_win(menu, nav_window);
    set_menu_sub(menu, derwin(nav_window, 1, WU * 7, 1, 1));
    set_menu_fore(menu, A_BOLD | A_ITALIC);
    set_menu_mark(menu, " > "); // Mark for the selected item

    // Post the menu (make it visible)
    post_menu(menu);

    // Refresh the left_inner_win window
    wrefresh(nav_window);
    return menu;
}

void init_right_panel_state(RIGHT_PANEL_STATE *rp_state,
                            bool course_view_active)
{
    rp_state->window_width = COLS - (WU * 7 + 4);
    rp_state->right_panel = newwin(course_view_active ? LINES - 3 : LINES,
                                   rp_state->window_width, 0, WU * 7 + 4);
    rp_state->inner_win = derwin(rp_state->right_panel, LINES - 8,
                                 rp_state->window_width - 6, 2, 4);
    rp_state->num_of_section_items[rp_state->curr_section] = 0;
    rp_state->showing_test_results = false;
    rp_state->showing_end_of_course_page = false;
}

void print_window_content(I_TEXT_BUFFER *header_tbuf, WINDOW *win,
                          int win_width)
{
    I_LINE *current_line = header_tbuf->first_line;

    int offset = 0;
    current_line = header_tbuf->first_line;

    for (int i = LINES > 55 ? 2 : 1; current_line != NULL; i++)
    {
        if (current_line->centered)
        {
            offset = (win_width - current_line->length - 1) / 2;
        }
        else
            offset = 0;
        if (current_line->style > 0)
        {
            wattron(win, current_line->style);
            mvwprintw(win, i, offset, "%s", current_line->buf_);
            wattroff(win, current_line->style);
        }
        else if (current_line->style == 0 && current_line->syntax_hl == false)
        {
            mvwprintw(win, i, offset, "%s", current_line->buf_);
            // mvwprintw(win, i, offset, "%i %i %i %i %li", offset,
            // current_line->length,
            //           win_width, CARD_WIDTH, strlen(current_line->buf_));
        }
        else if (current_line->syntax_hl == true)
        {
            print_line((LINE *)current_line, i, &win);
        }

        current_line = current_line->next;
    }
}
