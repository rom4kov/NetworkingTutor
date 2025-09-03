#include "../../../ntutor.h"
#include "../../core/core.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

WINDOW *create_greeter_screen(APP_CONTEXT *ctx)
{
    WINDOW *greeter_screen = newwin(LINES, COLS, 0, 0);
    ctx->greeter_ascii_window =
        derwin(greeter_screen, 10, 71, LINES / 5, (COLS - 71) / 2 + 2);
    draw_border(greeter_screen, 2, 0);
    wattron(greeter_screen, COLOR_PAIR(3) | A_BOLD);
    char *msg = "Welcome to";
    char *msg2 = "󰒍 NetworkingTutor v0.0.1";
    mvwprintw(greeter_screen, (LINES / 5) - 1, (COLS - strlen(msg)) / 2, "%s",
              msg);
    wattron(ctx->greeter_ascii_window, A_BOLD);
    mvwprintw(ctx->greeter_ascii_window, 0, 0, "%s",
              get_ascii_art(ctx->db, "logo"));
    wattroff(ctx->greeter_ascii_window, A_BOLD);

    ctx->greeter_menu = create_greeter_menu(greeter_screen);

    wattroff(greeter_screen, COLOR_PAIR(3) | A_BOLD);

    wattron(greeter_screen, COLOR_PAIR(3));
    mvwprintw(greeter_screen, (LINES / 2) + 14, (COLS - strlen(msg2)) / 2 + 2,
              msg2, LINES, COLS);
    wattroff(greeter_screen, COLOR_PAIR(3));
    wrefresh(ctx->greeter_ascii_window);

    return greeter_screen;
}

MENU *create_greeter_menu(WINDOW *greeter_screen)
{
    const char *choices[] = {
        "   🛫 Start new learning path",
        " 👉 Continue where you left off",
        "     👤 Create new account",
        "          🚀 Progress",
        "            Settings",
        "            Shortcuts",
        "            🚪 Quit",
        (char *)NULL // Last element must be NULL
    };

    ITEM **menu_items = (ITEM **)calloc(8, sizeof(ITEM *));

    for (int i = 0; choices[i] != NULL; i++)
    {
        menu_items[i] = new_item(choices[i], "");
    }

    // Create the menu
    MENU *menu = new_menu(menu_items);
    set_menu_format(menu, 14, 1);
    set_menu_spacing(menu, 0, 2, 2);
    // mvwprintw(nav_window, 2, 3, "%i", COLS);

    // Set the window for the menu to be displayed inside left_inner_win
    set_menu_win(menu, greeter_screen);
    set_menu_sub(menu, derwin(greeter_screen, 28, COLS / 6, (LINES / 5) * 2 + 3,
                              (COLS - 35) / 2 - 1));
    set_menu_fore(menu, A_BOLD | A_ITALIC);
    set_menu_mark(menu, " > "); // Mark for the selected item

    // Post the menu (make it visible)
    post_menu(menu);

    // Refresh the left_inner_win window
    wrefresh(greeter_screen);
    return menu;
}

MENU *create_start_options_menu(APP_CONTEXT *ctx, WINDOW **start_opt_menu_win,
                                WINDOW **start_opt_menu_sub, int num_of_users)
{
    char *choices[3] = {0};

    choices[0] = "    👤 Create new account";
    if (num_of_users > 0)
    {
        choices[1] = "  👉 Select existing account";
        choices[2] = (char *)NULL;
    }
    else
    {
        choices[1] = (char *)NULL;
    }

    ITEM **menu_items = (ITEM **)calloc(8, sizeof(ITEM *));

    for (int i = 0; choices[i] != NULL; i++)
    {
        menu_items[i] = new_item(choices[i], "");
    }

    // Create the menu
    MENU *menu = new_menu(menu_items);
    set_menu_format(menu, 14, 1);
    set_menu_spacing(menu, 0, 2, 2);
    // mvwprintw(nav_window, 2, 3, "%i", COLS);

    // Set the window for the menu to be displayed inside left_inner_win
    wclear(*start_opt_menu_win);
    draw_border(*start_opt_menu_win, 2, "Choose an option");
    set_menu_win(menu, *start_opt_menu_win);
    set_menu_sub(menu, *start_opt_menu_sub);
    set_menu_fore(menu, A_BOLD | A_ITALIC);
    set_menu_mark(menu, " > "); // Mark for the selected item

    // Post the menu (make it visible)
    post_menu(menu);

    // Refresh the left_inner_win window
    wrefresh(*start_opt_menu_win);
    return menu;
}

MENU *create_user_selection_menu(APP_CONTEXT *ctx,
                                 WINDOW **user_select_menu_win,
                                 WINDOW **user_select_sub, int num_of_users,
                                 int max_name_len)
{
    USER_DATA **users = malloc(sizeof(USER_DATA) * num_of_users);
    for (int i = 0; i < num_of_users; i++)
    {
        users[i] = get_user_data(ctx->db, i + 1);
    }

    ITEM **menu_items = (ITEM **)calloc(num_of_users + 1, sizeof(ITEM *));

    for (int i = 0; i < num_of_users; i++)
    {
        char *combined_str = calloc(40, 1);
        size_t len1 = strlen((char *)users[i]->name);
        memcpy(combined_str, users[i]->name, len1);
        memset(combined_str + len1, ' ', 29 - len1);
        memcpy(combined_str + 29, (char *)users[i]->created_at, 11);
        combined_str[39] = '\0';
        menu_items[i] = new_item(combined_str, "");
    }
    menu_items[num_of_users] = NULL;

    MENU *menu = new_menu(menu_items);
    set_menu_format(menu, num_of_users, 1);
    set_menu_spacing(menu, 8, 2, 0);

    wclear(*user_select_menu_win);
    draw_border(*user_select_menu_win, 2, "Choose an option");
    set_menu_win(menu, *user_select_menu_win);
    set_menu_sub(menu, *user_select_sub);
    set_menu_fore(menu, A_BOLD | A_ITALIC);
    set_menu_mark(menu, " > ");

    post_menu(menu);

    wrefresh(*user_select_menu_win);
    return menu;
}
