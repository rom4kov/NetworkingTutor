#include "../../../ntutor.h"
#include "../../core/core.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <panel.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void create_greeter_screen(APP_CONTEXT *ctx)
{
    // WINDOW *greeter_screen = newwin(LINES, COLS, 0, 0);
    ctx->num_of_users = get_user_count(ctx->db);
    int max_name_len = 0;

    ctx->greeter_windows[0] = newwin(LINES, COLS, 0, 0);
    ctx->greeter_panels[0] = new_panel(ctx->greeter_windows[0]);

    ctx->greeter_windows[1] =
        newwin(18, COLS / 6 + 2, (LINES / 5) * 2 + 1, (COLS - 35) / 2 - 3);
    newwin(18, COLS / 6 + 2, (LINES / 5) * 2 + 1, (COLS - 35) / 2 - 3);
    ctx->greeter_panels[1] = new_panel(ctx->greeter_windows[1]);

    ctx->greeter_menu = create_greeter_menu(ctx);

    ctx->greeter_windows[2] = create_start_options_popup(ctx);
    ctx->greeter_panels[2] = new_panel(ctx->greeter_windows[2]);

    ctx->greeter_windows[3] = derwin(ctx->greeter_windows[1], 5, 41, 2, 7);

    ctx->greeter_start_opts_menu =
        create_start_options_menu(ctx, &ctx->greeter_windows[2],
                                  &ctx->greeter_windows[2], ctx->num_of_users);

    ctx->greeter_windows[4] = newwin(7, 60, (LINES / 2) - 5, (COLS / 2) - 30);
    ctx->greeter_panels[3] = new_panel(ctx->greeter_windows[4]);

    ctx->greeter_windows[5] = derwin(ctx->greeter_windows[4], 1, 42, 2, 14);

    ctx->greeter_windows[6] =
        create_user_selection_popup(ctx, ctx->num_of_users, &max_name_len);
    ctx->greeter_panels[4] = new_panel(ctx->greeter_windows[6]);

    ctx->greeter_windows[7] =
        derwin(ctx->greeter_windows[6], ctx->num_of_users * 2, 44, 2, 2);


    ctx->greeter_ascii_window =
        derwin(ctx->greeter_windows[0], 10, 71, LINES / 5, (COLS - 71) / 2 + 2);
    draw_border(ctx->greeter_windows[0], 2, 0);
    wattron(ctx->greeter_windows[0], COLOR_PAIR(3) | A_BOLD);
    char *msg = "Welcome to";
    char *msg2 = "󰒍 NetworkingTutor v0.0.1";
    mvwprintw(ctx->greeter_windows[0], (LINES / 5) - 1,
              (COLS - strlen(msg)) / 2, "%s", msg);
    wattron(ctx->greeter_ascii_window, A_BOLD);
    mvwprintw(ctx->greeter_ascii_window, 0, 0, "%s",
              get_ascii_art(ctx->db, "logo"));
    wattroff(ctx->greeter_ascii_window, A_BOLD);

    wattroff(ctx->greeter_windows[0], COLOR_PAIR(3) | A_BOLD);

    wattron(ctx->greeter_windows[0], COLOR_PAIR(3));
    mvwprintw(ctx->greeter_windows[0], (LINES / 2) + 14,
              (COLS - strlen(msg2)) / 2 + 2, msg2, LINES, COLS);
    wattroff(ctx->greeter_windows[0], COLOR_PAIR(3));
    wnoutrefresh(ctx->greeter_ascii_window);

    keypad(ctx->greeter_windows[1], TRUE);
    ctx->active_window = ctx->greeter_windows[0];

    hide_panel(ctx->greeter_panels[2]);
    hide_panel(ctx->greeter_panels[3]);
    hide_panel(ctx->greeter_panels[4]);

    update_panels();
    doupdate();
}

MENU *create_greeter_menu(APP_CONTEXT *ctx)
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

    MENU *menu = new_menu(menu_items);
    set_menu_format(menu, 14, 1);
    set_menu_spacing(menu, 0, 2, 2);

    set_menu_win(menu, ctx->greeter_windows[1]);
    set_menu_sub(menu, derwin(ctx->greeter_windows[1], 16, COLS / 6, 1, 1));
    set_menu_fore(menu, A_BOLD | A_ITALIC);
    set_menu_mark(menu, " > ");

    post_menu(menu);
    return menu;
}

WINDOW *create_start_options_popup(APP_CONTEXT *ctx)
{
    WINDOW *start_opt_menu_win =
        newwin(7, 50, (LINES / 2) - 4, (COLS / 2) - 25);
    return start_opt_menu_win;
}

WINDOW *create_start_opt_popup_sub_win(APP_CONTEXT *ctx, int num_of_users)
{
    WINDOW *start_opt_menu_sub = derwin(ctx->greeter_windows[1], 5, 41, 2, 7);

    return start_opt_menu_sub;
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

    MENU *menu = new_menu(menu_items);
    set_menu_format(menu, 14, 1);
    set_menu_spacing(menu, 0, 2, 2);

    wclear(*start_opt_menu_win);
    draw_border(*start_opt_menu_win, 2, "Choose an option");
    set_menu_win(menu, ctx->greeter_windows[2]);
    ctx->greeter_windows[3] = derwin(ctx->greeter_windows[2], 3, 42, 2, 7);
    set_menu_sub(menu, ctx->greeter_windows[3]);
    set_menu_fore(menu, A_BOLD | A_ITALIC);
    set_menu_mark(menu, " > ");

    post_menu(menu);

    return menu;
}

FORM *create_new_user_popup_form(APP_CONTEXT *ctx, char *label)
{
    int rows, cols;

    ctx->new_user_form_field[0] = new_field(1, 32, 0, 2, 0, 0);
    ctx->new_user_form_field[1] = NULL;

    set_field_back(ctx->new_user_form_field[0], A_UNDERLINE);
    field_opts_off(ctx->new_user_form_field[0], O_AUTOSKIP);

    FORM *new_user_form = new_form(ctx->new_user_form_field);

    scale_form(ctx->new_user_form, &rows, &cols);

    set_form_win(ctx->new_user_form, ctx->greeter_windows[4]);
    set_form_sub(ctx->new_user_form, ctx->greeter_windows[5]);

    draw_border(ctx->greeter_windows[4], 1, "");
    mvwprintw(ctx->greeter_windows[4], 0, 2, "%s", label);
    mvwprintw(ctx->greeter_windows[4], 2, 3, "Your name: ");
    mvwprintw(ctx->greeter_windows[4], 4, 3,
              "Press ENTER to confirm, q to cancel");

    set_form_win(new_user_form, ctx->greeter_windows[4]);
    set_form_sub(new_user_form, ctx->greeter_windows[5]);

    wmove(ctx->greeter_windows[5], 0, 0);

    curs_set(1);
    set_current_field(new_user_form, ctx->new_user_form_field[0]);
    post_form(new_user_form);

    doupdate();

    return new_user_form;
}

WINDOW *create_user_selection_popup(APP_CONTEXT *ctx, int num_of_users,
                                    int *max_name_len)
{
    USER_DATA **users = malloc(sizeof(USER_DATA) * num_of_users);
    *max_name_len = 0;
    for (int i = 0; i < num_of_users; i++)
    {
        users[i] = get_user_data(ctx->db, i + 1);
        int name_len = strlen((char *)users[i]->name);
        if (name_len > *max_name_len)
            *max_name_len = name_len;
    }
    WINDOW *user_select_win =
        newwin(num_of_users * 2 + 3, 50, (LINES / 2) - 4, (COLS / 2) - 25);
    return user_select_win;
}

WINDOW *create_user_select_sub_win(APP_CONTEXT *ctx, int num_of_users,
                                   int *max_name_len)
{
    WINDOW *user_select_sub_win =
        derwin(ctx->greeter_windows[4], num_of_users * 2, 44, 2, 2);

    return user_select_sub_win;
}

MENU *create_user_selection_menu(APP_CONTEXT *ctx, int num_of_users,
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

    wclear(ctx->greeter_windows[6]);
    draw_border(ctx->greeter_windows[6], 2, "");
    mvwprintw(ctx->greeter_windows[6], 0, 3, " %s ", "Select a user");
    set_menu_win(menu, ctx->greeter_windows[6]);
    set_menu_sub(menu, ctx->greeter_windows[7]);
    set_menu_fore(menu, A_BOLD | A_ITALIC);
    set_menu_mark(menu, " > ");

    post_menu(menu);

    wnoutrefresh(ctx->greeter_windows[6]);
    return menu;
}
