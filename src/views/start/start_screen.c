#define _XOPEN_SOURCE_EXTENDED 1

#include "../../core/core.h"
#include "../../models/models.h"
#include "../views.h"
#include "start_menu.h"
#include "user_form.h"
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

char *HEADER_TEXT =
    "    _   __       __                          __    _              \n"
    "   / | / /___   / /_ _      __ ____   _____ / /__ (_)____   ____ _\n"
    "  /  |/ // _ \\ / __/| | /| / // __ \\ / ___// //_// // __ \\ / __ `/\n"
    " / /|  //  __// /_  | |/ |/ // /_/ // /   / ,<  / // / / // /_/ / \n"
    "/_/ |_/ \\___/ \\__/  |__/|__/ \\____//_/   /_/|_|/_//_/ /_/ \\__, /  \n"
    "  ______        __                                       /____/   \n"
    " /_  __/__  __ / /_ ____   _____                                  \n"
    "  / /  / / / // __// __ \\ / ___/                                  \n"
    " / /  / /_/ // /_ / /_/ // /                                      \n"
    "/_/   \\__,_/ \\__/ \\____//_/                                       \n";

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

char *HTTP = " _     _   _             ____\n"
             "| |__ | |_| |_ _ __ _   / / /\n"
             "| '_ \\| __| __| '_ (_) / / /\n"
             "| | | | |_| |_| |_) | / / / \n"
             "|_| |_|\\__|\\__| .__(_)_/_/  \n"
             "              |_|           \n";

void create_start_screen(APP_CONTEXT *ctx)
{
    ctx->start_windows[0] =
        create_navigation_window(&ctx->active_window, &ctx->start_menu);
    ctx->start_windows[1] = create_header_section(&ctx->active_window);
    ctx->start_windows[2] =
        create_course_preview_card(0, &ctx->active_window, 2, &ctx->courses[0]);
    ctx->start_windows[3] = create_course_preview_card(
        CARD_WIDTH, &ctx->active_window, 3, &ctx->courses[1]);
    ctx->start_windows[4] = create_course_preview_card(
        CARD_WIDTH * 2, &ctx->active_window, 4, &ctx->courses[2]);
    ctx->start_windows[5] = create_right_side_panel(ctx, "Details");

    wrefresh(ctx->start_windows[5]);
}

WINDOW *create_navigation_window(int *active_win, MENU **start_menu)
{
    WINDOW *navigation;
    navigation = newwin(3, WU * 7 + 4, 0, 0);
    draw_border(navigation, *active_win == 0 ? 3 : 2, "");
    *start_menu = create_start_menu(navigation);

    wattron(navigation, COLOR_PAIR(3));
    mvwprintw(navigation, 0, 2, "Navigation");
    wattroff(navigation, COLOR_PAIR(3));
    wrefresh(navigation);
    return navigation;
}

WINDOW *create_header_section(int *active_win)
{
    WINDOW *header_outer = newwin(LINES / 2 + 1, WU * 7 + 4, 3, 0);
    WINDOW *header_inner =
        derwin(header_outer, LINES / 2 - 1, CARD_WIDTH * 3 - 4, 0, 3);
    if (*active_win == 1)
    {
        wattron(header_outer, COLOR_PAIR(1) | A_BOLD);
    }
    draw_border(header_outer, 2, "Header");
    wattron(header_inner, A_BOLD);
    mvwprintw(header_inner, 1, 0, "%s", HEADER_TEXT);
    wattroff(header_inner, A_BOLD);
    mvwprintw(header_inner, 13, 0, "%s", PROGRAMM_DESC);
    if (*active_win == 1)
    {
        wattroff(header_inner, COLOR_PAIR(1) | A_BOLD);
    }
    wrefresh(header_outer);
    wrefresh(header_inner);
    return header_outer;
}

WINDOW *create_course_preview_card(int x_position, int *active_win,
                                   int curr_win_idx, COURSE *course)
{
    int remainder = 0;
    if (curr_win_idx == 4)
    {
        remainder = REMAINDER;
    }

    int height = LINES % 2 == 0 ? LINES / 2 - 4 : LINES / 2 - 3;
    WINDOW *course_preview_card_outer =
        newwin(height, CARD_WIDTH + remainder, LINES / 2 + 4, x_position);
    WINDOW *course_preview_card_inner =
        derwin(course_preview_card_outer, height - 2,
               CARD_WIDTH - 2 + remainder, 1, 1);
    WINDOW *description_window =
        derwin(course_preview_card_inner, height - 4, CARD_WIDTH - 4, 1, 2);
    mvwprintw(description_window, 12, 2, "a: %i c: %i", *active_win,
              curr_win_idx);

    if ((*active_win == 2 || *active_win == 3 || *active_win == 4) &&
        curr_win_idx == *active_win)
    {
        draw_border(course_preview_card_outer, 3, "Lesson");
    }
    else
    {
        draw_border(course_preview_card_outer, 2, "Lesson");
        mvwaddch(course_preview_card_outer, 2, 0, ACS_LTEE);
        mvwaddch(course_preview_card_outer, 2, CARD_WIDTH - 1 + remainder,
                 ACS_RTEE);
    }

    mvwhline(course_preview_card_inner, 1, 0, ACS_HLINE,
             CARD_WIDTH + remainder);
    wattron(course_preview_card_outer, COLOR_PAIR(3));
    mvwprintw(course_preview_card_outer, 0, 2, "Lesson #%i", course->id);
    wattroff(course_preview_card_outer, COLOR_PAIR(3));
    wrefresh(course_preview_card_outer);
    mvwprintw(course_preview_card_inner, 0,
              (CARD_WIDTH - strlen(course->name)) / 2 - 1, "%s", course->name);
    wrefresh(course_preview_card_inner);
    int y, x;
    getmaxyx(description_window, y, x);
    mvwprintw(description_window, 2, 0, "%s",
              wrap_text(course->short_desc, x - 8));
    if (curr_win_idx == 2)
    {
        mvwprintw(description_window, 9, 0, "%s", HTTP);
    }
    wrefresh(description_window);
    return course_preview_card_outer;
}

WINDOW *create_right_side_panel(APP_CONTEXT *ctx, char *label)
{
    init_right_panel_state(ctx->rp_state, ctx->course_view_active);

    if (ctx->active_window == 5)
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
        USER_DATA user_data = get_user_data(ctx->db);
        mvwprintw(ctx->rp_state->right_panel, 2, 3, "Your name: %s",
                  user_data.name);
        mvwprintw(ctx->rp_state->right_panel, 3, 3, "Language:  %s",
                  user_data.language);
        print_intro(&ctx->rp_state->right_panel, ctx->rp_state->window_width,
                    ctx->rp_state->intro_width);
    }
    else if (ctx->course_view_active)
    {
        print_course_instructions(ctx);
    }

    wnoutrefresh(ctx->rp_state->right_panel);
    wnoutrefresh(ctx->rp_state->header_win);
    wnoutrefresh(ctx->rp_state->inner_win);
    doupdate();
    return ctx->rp_state->right_panel;
}

MENU *create_start_menu(WINDOW *nav_window)
{
    const char *choices[] = {
        "Home",      "Courses", "Account", "Progress", "Settings", "Shortcuts",
        (char *)NULL // Last element must be NULL
    };

    ITEM **menu_items = (ITEM **)calloc(6, sizeof(ITEM *));

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
    rp_state->intro_width = rp_state->window_width - 10;

    rp_state->right_panel =
        newwin(course_view_active ? LINES - 3 : LINES, rp_state->window_width, 0, WU * 7 + 4);
    rp_state->header_win = derwin(rp_state->right_panel, 6, 31, 2,
                                  (rp_state->window_width - 30) / 2);
    rp_state->inner_win = derwin(rp_state->right_panel, LINES - 18,
                                 rp_state->window_width - 6, 13, 4);

    rp_state->curr_section = 0;
    rp_state->num_of_section_items = 0;
}
