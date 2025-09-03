#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <panel.h>
#include <string.h>

#define WU COLS / 12 // WU for WIDTH_UNIT
#define EXPLORER_WIDTH WU + WU / 2

void handle_greeter_input(APP_CONTEXT *ctx)
{
    ITEM *curr_item;

    switch (ctx->key)
    {
        case KEY_DOWN:
            menu_driver(ctx->greeter_menu, REQ_DOWN_ITEM);
            wrefresh(ctx->greeter_screen);
            break;
        case KEY_UP:
            menu_driver(ctx->greeter_menu, REQ_UP_ITEM);
            wrefresh(ctx->greeter_screen);
            break;
        case 10:
            curr_item = current_item(ctx->greeter_menu);
            if (item_index(curr_item) == 0)
            {
                create_start_options_popup(ctx);
                // create_new_user_popup(ctx);
            }
            else if (item_index(curr_item) == 1)
            {
                int current_course_id = get_current_course(ctx->db, 1);
                go_to_course_by_id(ctx, current_course_id);
            }
            else if (item_index(curr_item) == 6)
            {
                ctx->greeter_view_active = false;
                ctx->running = false;
            }
            break;
        case 'q':
        case 27:
            ctx->greeter_view_active = false;
            ctx->running = false;
            break;
    }
}

void create_new_user_popup_form(WINDOW **inner_win, WINDOW **form_window,
                                FORM **new_file_form, FIELD **field,
                                char *label)
{
    int rows, cols;

    field[0] = new_field(1, 32, 0, 2, 0, 0);
    field[1] = NULL;

    set_field_back(field[0], A_UNDERLINE);
    field_opts_off(field[0], O_AUTOSKIP);

    *new_file_form = new_form(field);

    scale_form(*new_file_form, &rows, &cols);

    set_form_win(*new_file_form, *inner_win);
    set_form_sub(*new_file_form, *form_window);

    draw_border(*inner_win, 1, "");
    mvwprintw(*inner_win, 0, 2, "%s", label);
    mvwprintw(*inner_win, 2, 3, "Your name: ");
    mvwprintw(*inner_win, 4, 3, "Press ENTER to confirm, q to cancel");

    set_form_win(*new_file_form, *inner_win);
    set_form_sub(*new_file_form, *form_window);

    wclear(*form_window);

    wmove(*form_window, 0, 0);

    curs_set(1);
    set_current_field(*new_file_form, field[0]);
    post_form(*new_file_form);

    wnoutrefresh(*inner_win);
    wnoutrefresh(*form_window);
    doupdate();
}

void create_new_user_popup(APP_CONTEXT *ctx)
{
    WINDOW *inner_win = newwin(7, 60, (LINES / 2) - 5, (COLS / 2) - 30);
    WINDOW *form_window = derwin(inner_win, 1, 42, 2, 14);
    FORM *new_file_form = NULL;
    FIELD *field[2];
    char *username;

    create_new_user_popup_form(&inner_win, &form_window, &new_file_form, field,
                               " Create new user ");

    bool create_user_form_active = true;

    while (create_user_form_active)
    {
        ctx->key = wgetch(ctx->greeter_screen);

        switch (ctx->key)
        {
            case 263: // Backspace
                form_driver(new_file_form, REQ_VALIDATION);
                FIELD *current = current_field(new_file_form);
                char *buf = field_buffer(current, 0);
                trim(&buf);
                if (buf && strlen(buf) > 0)
                {
                    form_driver(new_file_form, REQ_DEL_PREV);
                    wrefresh(form_window);
                }
                break;
            case 10:
                form_driver(new_file_form, REQ_VALIDATION);
                username = field_buffer(field[0], 0);
                trim(&username);

                ctx->current_user_id = create_new_user(ctx, username);
                ctx->key = wgetch(ctx->greeter_screen);

                create_user_form_active = false;
                werase(inner_win);
                werase(form_window);
                curs_set(0);
                unpost_form(new_file_form);
                free_form(new_file_form);
                free_field(field[0]);
                ctx->greeter_screen = create_greeter_screen(ctx);
                doupdate();
                ctx->greeter_view_active = false;
                ctx->start_view_active = true;
                ctx->start_needs_redraw = true;
                ctx->rp_state->sections_completed = 0;
                ctx->rp_state->items_completed = 0;
                ctx->rp_state->curr_section = 0;
                ctx->rp_state->curr_item = 2;
                break;
            case 'q':
                create_user_form_active = false;
                werase(inner_win);
                werase(form_window);
                wrefresh(inner_win);
                wrefresh(form_window);
                menu_driver(ctx->greeter_menu, REQ_NEXT_ITEM);
                menu_driver(ctx->greeter_menu, REQ_PREV_ITEM);
                doupdate();
                curs_set(0);
                unpost_form(new_file_form);
                free_form(new_file_form);
                free_field(field[0]);
                doupdate();
                break;
            default:
                form_driver(new_file_form, ctx->key);
                wrefresh(form_window);
                break;
        }
    }
}

void create_start_options_popup(APP_CONTEXT *ctx)
{
    int num_of_users = get_user_count(ctx->db);

    WINDOW *start_opt_menu_win =
        derwin(ctx->greeter_screen, 7, 50, (LINES / 2) - 4, (COLS / 2) - 25);
    WINDOW *start_opt_menu_sub = derwin(start_opt_menu_win, 5, 41, 2, 7);
    MENU *start_options_menu = create_start_options_menu(
        ctx, &start_opt_menu_win, &start_opt_menu_sub, num_of_users);
    ITEM *curr_item = NULL;

    bool start_opt_menu_active = true;

    while (start_opt_menu_active)
    {
        keypad(start_opt_menu_sub, TRUE);
        ctx->key = wgetch(start_opt_menu_sub);

        switch (ctx->key)
        {
            case KEY_DOWN: // Backspace
                menu_driver(start_options_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP: // Backspace
                menu_driver(start_options_menu, REQ_UP_ITEM);
                break;
            case 10:
                curr_item = current_item(start_options_menu);
                if (item_index(curr_item) == 0)
                {
                    create_new_user_popup(ctx);
                }
                else if (item_index(curr_item) == 1)
                {
                    create_user_selection_popup(
                        ctx, num_of_users, start_options_menu,
                        start_opt_menu_win, start_opt_menu_sub);
                    menu_driver(start_options_menu, REQ_PREV_ITEM);
                    menu_driver(start_options_menu, REQ_NEXT_ITEM);
                    werase(start_opt_menu_win);
                    wrefresh(start_opt_menu_win);
                }
                break;
            case 'q':
                start_opt_menu_active = false;
                wclear(start_opt_menu_win);
                wclear(start_opt_menu_sub);
                wnoutrefresh(start_opt_menu_win);
                wnoutrefresh(start_opt_menu_sub);
                menu_driver(ctx->greeter_menu, REQ_NEXT_ITEM);
                menu_driver(ctx->greeter_menu, REQ_PREV_ITEM);
                doupdate();
                break;
            default:
                break;
        }
    }
}

void create_user_selection_popup(APP_CONTEXT *ctx, int num_of_users,
                                 MENU *start_opt_menu, WINDOW *start_opts_win,
                                 WINDOW *start_opts_sub)
{

    USER_DATA **users = malloc(sizeof(USER_DATA) * num_of_users);
    int max_name_len = 0;
    for (int i = 0; i < num_of_users; i++)
    {
        users[i] = get_user_data(ctx->db, i + 1);
        int name_len = strlen((char *)users[i]->name);
        if (name_len > max_name_len)
            max_name_len = name_len;
    }
    WINDOW *user_select_win = derwin(ctx->greeter_screen, num_of_users * 2 + 4,
                                     50, (LINES / 2) - 4, (COLS / 2) - 25);
    WINDOW *user_select_sub_win =
        derwin(user_select_win, num_of_users * 2, 44, 2, 2);
    MENU *user_select_menu =
        create_user_selection_menu(ctx, &user_select_win, &user_select_sub_win,
                                   num_of_users, max_name_len);
    ITEM *curr_item = NULL;

    bool user_select_menu_active = true;

    while (user_select_menu_active)
    {
        keypad(user_select_sub_win, TRUE);
        ctx->key = wgetch(user_select_sub_win);

        switch (ctx->key)
        {
            case KEY_DOWN: // Backspace
                menu_driver(user_select_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP: // Backspace
                menu_driver(user_select_menu, REQ_UP_ITEM);
                break;
            case '\n':
                curr_item = current_item(user_select_menu);

                ctx->current_user_id = item_index(curr_item) + 1;
                ctx->user_data = get_user_data(ctx->db, ctx->current_user_id);

                user_select_menu_active = false;
                wclear(ctx->greeter_screen);
                wrefresh(ctx->greeter_screen);
                // werase(inner_win);
                // werase(form_window);
                // curs_set(0);
                // unpost_form(new_file_form);
                // free_form(new_file_form);
                // free_field(field[0]);
                ctx->greeter_view_active = false;
                ctx->start_view_active = true;
                ctx->start_needs_redraw = true;
                ctx->rp_state->sections_completed = 0;
                ctx->rp_state->items_completed = 0;
                ctx->rp_state->curr_section = 0;
                ctx->rp_state->curr_item = 2;
                break;
            case 'q':
                user_select_menu_active = false;
                wclear(user_select_win);
                wclear(user_select_sub_win);
                wnoutrefresh(user_select_win);
                wnoutrefresh(user_select_sub_win);
                wnoutrefresh(start_opts_win);
                wnoutrefresh(start_opts_sub);
                unpost_menu(user_select_menu);
                free_menu(user_select_menu);
                keypad(start_opts_sub, TRUE);
                menu_driver(start_opt_menu, REQ_PREV_ITEM);
                menu_driver(start_opt_menu, REQ_NEXT_ITEM);
                doupdate();
                curs_set(0);
                break;
            default:
                // form_driver(new_file_form, ctx->key);
                // wrefresh(form_window);
                break;
        }
    }
}
