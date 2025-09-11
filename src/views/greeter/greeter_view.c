#include "../../../ntutor.h"
#include "../../core/core.h"
#include "../../models/models.h"
#include "../../views/views.h"
#include <curses.h>
#include <form.h>
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

    ctx->greeter_windows[0] = newwin(LINES, COLS, 0, 0);
    ctx->greeter_panels[0] = new_panel(ctx->greeter_windows[0]);

    ctx->greeter_windows[1] =
        newwin(18, COLS / 6 + 2, (LINES / 5) * 2 + 1, (COLS - 35) / 2 - 3);
    ctx->greeter_panels[1] = new_panel(ctx->greeter_windows[1]);

    ctx->greeter_menu = create_greeter_menu(ctx);

    ctx->greeter_windows[2] = create_start_options_popup();
    ctx->greeter_panels[2] = new_panel(ctx->greeter_windows[2]);
    ctx->greeter_windows[3] = derwin(ctx->greeter_windows[1], 5, 41, 2, 7);
    ctx->greeter_start_opts_menu = create_start_options_menu(
        ctx, &ctx->greeter_windows[2], ctx->num_of_users);

    ctx->greeter_windows[4] = newwin(7, 60, (LINES / 2) - 5, (COLS / 2) - 30);
    ctx->greeter_panels[3] = new_panel(ctx->greeter_windows[4]);
    ctx->greeter_windows[5] = derwin(ctx->greeter_windows[4], 1, 42, 2, 14);
    ctx->new_user_form =
        create_new_user_popup_form(ctx, " Create new user ");

    ctx->greeter_windows[6] =
        newwin(ctx->num_of_users * 2 + 5, 50, (LINES / 2) - 4, (COLS / 2) - 25);
    ctx->greeter_panels[4] = new_panel(ctx->greeter_windows[6]);
    ctx->greeter_windows[7] =
        derwin(ctx->greeter_windows[6], ctx->num_of_users * 2, 44, 2, 2);
    ctx->greeter_user_select_menu =
        create_user_selection_menu(ctx, ctx->num_of_users);

    ctx->greeter_ascii_window =
        derwin(ctx->greeter_windows[0], 10, 71, LINES / 5, (COLS - 71) / 2 + 2);

    wattron(ctx->greeter_ascii_window, A_BOLD);
    char *greeter_ascii = get_ascii_art(ctx->db, "logo");
    mvwprintw(ctx->greeter_ascii_window, 0, 0, "%s", greeter_ascii);
    wattroff(ctx->greeter_ascii_window, A_BOLD);
    free(greeter_ascii);

    draw_border(ctx->greeter_windows[0], 2, 0);

    wattron(ctx->greeter_windows[0], COLOR_PAIR(3) | A_BOLD);
    char *msg = "Welcome to";
    char *msg2 = "󰒍 NetworkingTutor v0.0.1";
    mvwprintw(ctx->greeter_windows[0], (LINES / 5) - 1,
              (COLS - strlen(msg)) / 2, "%s", msg);

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
        "   🛫 Start new learning path", " 👉 Continue where you left off",
        "     👤 Create new account",    "          🚀 Progress",
        "            Settings",       "            Shortcuts",
        "            🚪 Quit",           (char *)NULL};

    ctx->greeter_menu_items = (ITEM **)calloc(8, sizeof(ITEM *));

    for (int i = 0; choices[i] != NULL; i++)
    {
        ctx->greeter_menu_items[i] = new_item(choices[i], "");
    }

    MENU *menu = new_menu(ctx->greeter_menu_items);
    set_menu_format(menu, 14, 1);
    set_menu_spacing(menu, 0, 2, 2);

    set_menu_win(menu, ctx->greeter_windows[1]);
    set_menu_sub(menu, derwin(ctx->greeter_windows[1], 16, COLS / 6, 1, 1));
    set_menu_fore(menu, A_BOLD | A_ITALIC);
    set_menu_mark(menu, " > ");

    post_menu(menu);

    for (int i = 0; ctx->greeter_menu_items[i] != NULL; i++)
    {
        free_item(ctx->greeter_menu_items[i]);
    }

    return menu;
}

WINDOW *create_start_options_popup()
{
    WINDOW *start_opt_menu_win =
        newwin(9, 50, (LINES / 2) - 4, (COLS / 2) - 25);
    return start_opt_menu_win;
}

MENU *create_start_options_menu(APP_CONTEXT *ctx, WINDOW **start_opt_menu_win,
                                int num_of_users)
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

    ctx->greeter_start_opts_menu_items = (ITEM **)calloc(8, sizeof(ITEM *));

    for (int i = 0; choices[i] != NULL; i++)
    {
        ctx->greeter_start_opts_menu_items[i] = new_item(choices[i], "");
    }

    MENU *menu = new_menu(ctx->greeter_start_opts_menu_items);
    set_menu_format(menu, 14, 1);
    set_menu_spacing(menu, 0, 2, 2);

    wclear(*start_opt_menu_win);
    draw_border(*start_opt_menu_win, 2, "Choose an option");

    set_menu_win(menu, ctx->greeter_windows[2]);
    ctx->greeter_windows[3] = derwin(ctx->greeter_windows[2], 3, 42, 2, 7);
    set_menu_sub(menu, ctx->greeter_windows[3]);
    set_menu_fore(menu, A_BOLD | A_ITALIC);
    set_menu_mark(menu, " > ");

    mvwprintw(ctx->greeter_windows[2], 6, 15,
              " Press q to cancel ");

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

    scale_form(new_user_form, &rows, &cols);

    set_form_win(new_user_form, ctx->greeter_windows[4]);
    set_form_sub(new_user_form, ctx->greeter_windows[5]);

    draw_border(ctx->greeter_windows[4], 1, "");
    mvwprintw(ctx->greeter_windows[4], 0, 2, "%s", label);
    mvwprintw(ctx->greeter_windows[4], 2, 3, "Your name: ");
    mvwprintw(ctx->greeter_windows[4], 4, 3,
              "Press ENTER to confirm, F1 to cancel");

    wmove(ctx->greeter_windows[4], 0, 0);

    set_current_field(new_user_form, ctx->new_user_form_field[0]);
    post_form(new_user_form);

    doupdate();

    return new_user_form;
}

MENU *create_user_selection_menu(APP_CONTEXT *ctx, int num_of_users)
{
    USER_DATA **users = malloc(sizeof(USER_DATA) * num_of_users);
    for (int i = 0; i < num_of_users; i++)
    {
        users[i] = get_user_data(ctx->db, i + 1);
    }

    ctx->greeter_user_select_menu_items =
        (ITEM **)calloc(num_of_users + 1, sizeof(ITEM *));
    ctx->user_select_menu_strings =
        (char **)calloc(num_of_users, sizeof(char *));

    for (int i = 0; i < num_of_users; i++)
    {
        ctx->user_select_menu_strings[i] = (char *)calloc(40, 1);
        size_t len1 = strlen((char *)users[i]->name);
        memcpy(ctx->user_select_menu_strings[i], users[i]->name, len1);
        memset(ctx->user_select_menu_strings[i] + len1, ' ', 29 - len1);
        memcpy(ctx->user_select_menu_strings[i] + 29,
               (char *)users[i]->created_at, 11);
        ctx->user_select_menu_strings[i][39] = '\0';
        ctx->greeter_user_select_menu_items[i] =
            new_item(ctx->user_select_menu_strings[i], "");
    }
    ctx->greeter_user_select_menu_items[num_of_users] = NULL;

    for (int i = 0; i < num_of_users; i++)
    {
        free((void *)users[i]->name);
        free((void *)users[i]->created_at);
        free(users[i]);
    }
    free(users);

    MENU *menu = new_menu(ctx->greeter_user_select_menu_items);
    set_menu_format(menu, num_of_users, 1);
    set_menu_spacing(menu, 8, 2, 0);

    wclear(ctx->greeter_windows[6]);
    draw_border(ctx->greeter_windows[6], 2, "");
    mvwprintw(ctx->greeter_windows[6], 0, 3, " %s ", "Select a user");
    mvwprintw(ctx->greeter_windows[6], num_of_users * 2 + 2, 15, " %s ", "Press q to cancel");
    set_menu_win(menu, ctx->greeter_windows[6]);
    set_menu_sub(menu, ctx->greeter_windows[7]);
    set_menu_fore(menu, A_BOLD | A_ITALIC);
    set_menu_mark(menu, " > ");

    post_menu(menu);

    wnoutrefresh(ctx->greeter_windows[6]);
    return menu;
}

void deallocate_greeter_memory(APP_CONTEXT *ctx)
{
    unpost_menu(ctx->greeter_menu);
    unpost_menu(ctx->greeter_start_opts_menu);
    unpost_form(ctx->new_user_form);
    unpost_menu(ctx->greeter_user_select_menu);
    free_menu(ctx->greeter_menu);
    free_menu(ctx->greeter_start_opts_menu);
    free_form(ctx->new_user_form);
    free_menu(ctx->greeter_user_select_menu);

    free_field(ctx->new_user_form_field[0]);

    for (int i = 0; ctx->greeter_menu_items[i] != NULL; i++)
    {
        free_item(ctx->greeter_menu_items[i]);
    }
    free(ctx->greeter_menu_items);

    for (int i = 0; ctx->greeter_start_opts_menu_items[i] != NULL; i++)
    {
        free_item(ctx->greeter_start_opts_menu_items[i]);
    }
    free(ctx->greeter_start_opts_menu_items);

    for (int i = 0; ctx->greeter_user_select_menu_items[i] != NULL; i++)
    {
        free_item(ctx->greeter_user_select_menu_items[i]);
    }
    free(ctx->greeter_user_select_menu_items);

    for (int i = 0; i < ctx->num_of_users; i++)
    {
        free(ctx->user_select_menu_strings[i]);
    }
    free(ctx->user_select_menu_strings);

    for (int i = 0; i < GREETER_PANEL_COUNT; i++)
    {
        del_panel(ctx->greeter_panels[i]);
    }

    for (int i = 0; i < GREETER_WINDOW_COUNT; i++)
    {
        delwin(ctx->greeter_windows[i]);
    }
}
