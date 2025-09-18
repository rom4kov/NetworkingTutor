#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"
#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <panel.h>
#include <string.h>
#include <unistd.h>

void handle_greeter_input(APP_CONTEXT *ctx)
{
    ITEM *curr_item;
    bool start_opt_menu_active = false;

    switch (ctx->key)
    {
        case KEY_DOWN:
            menu_driver(ctx->greeter_menu, REQ_DOWN_ITEM);
            wnoutrefresh(ctx->greeter_windows[1]);
            doupdate();
            break;
        case KEY_UP:
            menu_driver(ctx->greeter_menu, REQ_UP_ITEM);
            wnoutrefresh(ctx->greeter_windows[1]);
            doupdate();
            break;
        case 10:
            curr_item = current_item(ctx->greeter_menu);
            if (item_index(curr_item) == 0)
            {
                if (ctx->num_of_users > 0)
                {
                    ctx->active_window = ctx->greeter_windows[2];
                    top_panel(ctx->greeter_panels[2]);
                    update_panels();
                    doupdate();
                    handle_start_opts_menu_input(ctx,
                                                 ctx->greeter_start_opts_menu);
                }
                else
                {
                    ctx->active_window = ctx->greeter_windows[4];
                    top_panel(ctx->greeter_panels[3]);
                    update_panels();
                    doupdate();
                    handle_new_user_input(ctx, &start_opt_menu_active);
                }
            }
            else if (item_index(curr_item) == 1)
            {
                if (ctx->num_of_users > 1)
                {
                    ctx->active_window = ctx->greeter_windows[6];
                    top_panel(ctx->greeter_panels[4]);
                    update_panels();
                    doupdate();
                    handle_user_select_win_input(ctx, &start_opt_menu_active,
                                                 1);
                }
                else
                {
                    int user_id = get_id_of_first_user(ctx->db);
                    if (user_id < 1)
                    {
                        mvwprintw(ctx->greeter_windows[1],
                                  ctx->num_of_users * 2 + 2, 2, "%s",
                                  "No user found");
                        wrefresh(ctx->greeter_windows[1]);
                        break;
                    }
                    ctx->current_user_id = get_id_of_first_user(ctx->db);
                    ctx->current_course_id =
                        get_current_course(ctx->db, ctx->current_user_id);
                    ctx->user_data =
                        get_user_data(ctx->db, ctx->current_user_id);
		    
                    chdir(ctx->user_data->home_dir);
                    go_to_course_by_id(ctx, ctx->current_course_id);
                    // deallocate_greeter_memory(ctx);
                }
            }
            else if (item_index(curr_item) == 2)
            {
                if (ctx->num_of_users > 1)
                {
                    ctx->active_window = ctx->greeter_windows[6];
                    top_panel(ctx->greeter_panels[4]);
                    update_panels();
                    doupdate();
                    handle_user_select_win_input(ctx, &start_opt_menu_active,
                                                 2);
                }
                else if (ctx->num_of_users == 1)
                {
                    ctx->greeter_needs_redraw = false;
                    ctx->greeter_view_active = false;
                    ctx->all_courses_needs_redraw = true;
                    ctx->all_courses_view_active = true;
                    ctx->current_user_id = get_id_of_first_user(ctx->db);
                    ctx->current_course_id =
                        get_current_course(ctx->db, ctx->current_user_id);
                    ctx->user_data =
                        get_user_data(ctx->db, ctx->current_user_id);
                    chdir(ctx->user_data->home_dir);
                    ctx->current_course_id = 1;
                    ctx->current_course = ctx->courses[0].name;
                    // deallocate_greeter_memory(ctx);
                }
            }
            else if (item_index(curr_item) == 3)
            {
                if (ctx->num_of_users > 1)
                {
                    ctx->active_window = ctx->greeter_windows[6];
                    top_panel(ctx->greeter_panels[4]);
                    update_panels();
                    doupdate();
                    handle_user_select_win_input(ctx, &start_opt_menu_active,
                                                 3);
                }
            }
            else if (item_index(curr_item) == 4)
            {
                if (ctx->num_of_users > 1)
                {
                    ctx->active_window = ctx->greeter_windows[6];
                    top_panel(ctx->greeter_panels[4]);
                    update_panels();
                    doupdate();
                    handle_user_select_win_input(ctx, &start_opt_menu_active,
                                                 4);
                }
            }
            else if (item_index(curr_item) == 5)
            {
                ctx->greeter_view_active = false;
                ctx->running = false;
            }
            break;
        case 'q':
        case 27:
            ctx->running = false;
            break;
    }
}

void handle_start_opts_menu_input(APP_CONTEXT *ctx, MENU *start_options_menu)
{
    ITEM *curr_item = NULL;
    bool start_opt_menu_active = true;

    while (start_opt_menu_active)
    {
        keypad(ctx->greeter_windows[3], TRUE);
        ctx->key = wgetch(ctx->greeter_windows[3]);

        switch (ctx->key)
        {
            case KEY_DOWN:
                menu_driver(start_options_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(start_options_menu, REQ_UP_ITEM);
                break;
            case 10:
                curr_item = current_item(start_options_menu);
                if (item_index(curr_item) == 0)
                {
                    ctx->active_window = ctx->greeter_windows[4];
                    top_panel(ctx->greeter_panels[3]);
                    update_panels();
                    doupdate();
                    handle_new_user_input(ctx, &start_opt_menu_active);
                }
                else if (item_index(curr_item) == 1)
                {
                    ctx->active_window = ctx->greeter_windows[6];
                    top_panel(ctx->greeter_panels[4]);
                    update_panels();
                    doupdate();
                    handle_user_select_win_input(ctx, &start_opt_menu_active, 0);
                }
                break;
            case 'q':
                start_opt_menu_active = false;
                hide_panel(ctx->greeter_panels[2]);
                top_panel(ctx->greeter_panels[1]);
                wnoutrefresh(ctx->greeter_windows[1]);
                update_panels();
                ctx->active_window = ctx->greeter_windows[1];

                doupdate();
                break;
            default:
                break;
        }
    }
}

void handle_new_user_input(APP_CONTEXT *ctx, bool *start_opt_menu_active)
{
    bool create_user_form_active = true;

    char *username;

    curs_set(1);

    while (create_user_form_active)
    {
        keypad(ctx->greeter_windows[4], TRUE);
        ctx->key = wgetch(ctx->greeter_windows[4]);

        switch (ctx->key)
        {
            case 263: // Backspace
                form_driver(ctx->new_user_form, REQ_VALIDATION);
                FIELD *current = current_field(ctx->new_user_form);
                char *buf = field_buffer(current, 0);
                trim(&buf);
                if (buf && strlen(buf) > 0)
                {
                    form_driver(ctx->new_user_form, REQ_DEL_PREV);
                    wrefresh(ctx->greeter_windows[4]);
                }
                break;
            case 10:
                form_driver(ctx->new_user_form, REQ_VALIDATION);
                username = field_buffer(ctx->new_user_form_field[0], 0);
                trim(&username);

                ctx->current_user_id = create_new_user(ctx, username);
                ctx->user_data = get_user_data(ctx->db, ctx->current_user_id);

                create_app_root_dir(ctx);

                ctx->current_course_id = 1;
                ctx->current_course = ctx->courses[0].name;

                create_user_form_active = false;
                *start_opt_menu_active = false;
                werase(ctx->greeter_windows[4]);
                werase(ctx->greeter_windows[5]);
                curs_set(0);
                cleanup_greeter_for_switch(ctx);
                doupdate();
                ctx->greeter_view_active = false;
                ctx->start_view_active = true;
                ctx->start_needs_redraw = true;
                ctx->rp_state->sections_completed = 0;
                ctx->rp_state->items_completed = 0;
                ctx->rp_state->curr_section = 0;
                ctx->rp_state->curr_item = 2;
                break;
            case KEY_F(1):
                create_user_form_active = false;

                ctx->active_window = ctx->greeter_windows[2];
                curs_set(0);
                hide_panel(ctx->greeter_panels[3]);
                top_panel(ctx->greeter_panels[2]);

                update_panels();
                doupdate();
                // deallocate_greeter_memory(ctx);

                break;
            default:
                form_driver(ctx->new_user_form, ctx->key);
                wrefresh(ctx->greeter_windows[4]);
                wrefresh(ctx->greeter_windows[5]);
                break;
        }
    }
}

void handle_user_select_win_input(APP_CONTEXT *ctx, bool *start_opt_menu_active,
                                  int menu_option)
{
    ITEM *curr_item = NULL;

    bool user_select_menu_active = true;

    while (user_select_menu_active)
    {
        keypad(ctx->greeter_windows[6], TRUE);
        ctx->key = wgetch(ctx->greeter_windows[6]);

        switch (ctx->key)
        {
            case KEY_DOWN:
                menu_driver(ctx->greeter_user_select_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(ctx->greeter_user_select_menu, REQ_UP_ITEM);
                break;
            case '\n':
                curr_item = current_item(ctx->greeter_user_select_menu);

                ctx->current_user_id = item_index(curr_item) + 1;
                ctx->current_course_id =
                    get_current_course(ctx->db, ctx->current_user_id);

                user_select_menu_active = false;
                *start_opt_menu_active = false;
                wclear(ctx->greeter_windows[0]);
                wrefresh(ctx->greeter_windows[0]);

                ctx->greeter_needs_redraw = false;
                ctx->greeter_view_active = false;
                if (menu_option == 0)
                {
                    ctx->user_data =
                        get_user_data(ctx->db, ctx->current_user_id);
                    chdir(ctx->user_data->home_dir);
                    ctx->start_view_active = true;
                    ctx->start_needs_redraw = true;
                }
                else if (ctx->num_of_users > 0)
                {
                    if (menu_option == 1)
                    {
                        ctx->user_data =
                            get_user_data(ctx->db, ctx->current_user_id);
                        chdir(ctx->user_data->home_dir);
                        go_to_course_by_id(ctx, ctx->current_course_id);
                    }
                    else if (menu_option == 2)
                    {
                        ctx->user_data =
                            get_user_data(ctx->db, ctx->current_user_id);
                        chdir(ctx->user_data->home_dir);
                        ctx->all_courses_view_active = true;
                        ctx->all_courses_needs_redraw = true;
                    }
                    else if (menu_option == 3)
                    {
                        ctx->user_data =
                            get_user_data(ctx->db, ctx->current_user_id);
                        chdir(ctx->user_data->home_dir);
                        ctx->progress_view_active = true;
                        ctx->progress_needs_redraw = true;
                    }
                    else if (menu_option == 4)
                    {
                        ctx->user_data =
                            get_user_data(ctx->db, ctx->current_user_id);
                        chdir(ctx->user_data->home_dir);
                        ctx->keybindings_view_active = true;
                        ctx->keybindings_needs_redraw = true;
                    }
                }
                cleanup_greeter_for_switch(ctx);
                break;
            case 'q':
                user_select_menu_active = false;

                if (menu_option == 1)
                {
                    ctx->active_window = ctx->greeter_windows[1];
                    hide_panel(ctx->greeter_panels[4]);
                    top_panel(ctx->greeter_panels[1]);
                }
                else
                {
                    ctx->active_window = ctx->greeter_windows[2];
                    hide_panel(ctx->greeter_panels[4]);
                    top_panel(ctx->greeter_panels[2]);
                }

                update_panels();
                doupdate();
                break;
            default:
                break;
        }
    }
}
