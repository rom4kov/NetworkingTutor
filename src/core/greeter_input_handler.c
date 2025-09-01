#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
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
                ctx->greeter_view_active = false;
                ctx->start_view_active = true;
                ctx->start_needs_redraw = true;
                ctx->rp_state->sections_completed = 0;
                ctx->rp_state->items_completed = 0;
                ctx->rp_state->curr_section = 0;
                ctx->rp_state->curr_item = 2;
                create_new_user_popup(ctx);
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
    mvwprintw(*inner_win, 4, 3,
              "Press ENTER to confirm and start your journey");

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
                // mvwprintw(inner_win, 5, 2, "%s", username);
                // wrefresh(inner_win);

                ctx->current_user_id = create_new_user(ctx, username);
                ctx->key = wgetch(ctx->greeter_screen);

                // if (strcmp(answer, "y") == 0)
                // {
                create_user_form_active = false;
                //     ctx->explorer_mode = true;
                //     ctx->editor_mode = false;
                //     ctx->active_window = 1;
                //
                //     if (strcmp(ctx->file_tree->current_entry->name,
                //                ctx->filename) == 0)
                //     {
                wclear(inner_win);
                wclear(form_window);
                //         wclear(ctx->course_windows[2]);
                //         wclear(ctx->edit_window);
                //         ctx->course_windows[2] =
                //             create_editor_window();
                //
                //         wattron(ctx->course_windows[2], COLOR_PAIR(10));
                //         print_no_open_file_msg(&ctx->course_windows[2]);
                //         wattroff(ctx->course_windows[2], COLOR_PAIR(10));
                //
                //         deallocate_buffer(ctx->t_buffer);
                //         ctx->t_buffer = initialize_buffer();
                //     }
                //
                //     if (ctx->file && ctx->file->_fileno > 0)
                //         fclose(ctx->file);
                //     // mvwprintw(ctx->course_windows[3], 15, 50, "%s",
                //     //           ctx->file_tree->current_entry->path);
                //     // wrefresh(ctx->course_windows[3]);
                //     remove(ctx->file_tree->current_entry->path);
                //
                //     remove_entry_from_file_tree(ctx->file_tree);
                //
                curs_set(0);
                unpost_form(new_file_form);
                free_form(new_file_form);
                free_field(field[0]);
                //
                //     wclear(ctx->course_windows[1]);
                //     ctx->course_windows[1] =
                //         create_explorer_window(ctx->file_tree);
                //     ctx->course_windows[2] =
                //         create_editor_window();
                //
                //     focus_window(&ctx->course_windows[2], 2, "Editor");
                //     focus_window(&ctx->course_windows[1], 3, "Explorer");
                //
                //     wattron(ctx->course_windows[2], COLOR_PAIR(10));
                //     print_no_open_file_msg(&ctx->course_windows[2]);
                //     wattroff(ctx->course_windows[2], COLOR_PAIR(10));
                //
                //     wnoutrefresh(ctx->course_windows[1]);
                //     wnoutrefresh(ctx->line_num_win);
                //     wnoutrefresh(ctx->edit_window);
                //     wnoutrefresh(ctx->course_windows[2]);
                //     doupdate();
                // }
                // else
                // {
                //     *create_user_form_active = false;
                //     curs_set(0);
                //     unpost_form(new_file_form);
                //     free_form(new_file_form);
                //     free_field(field[0]);
                //     menu_driver(ctx->explorer_menu, REQ_NEXT_ITEM);
                //     ctx->course_windows[1] =
                //         create_explorer_window(ctx->file_tree);
                //     focus_window(&ctx->course_windows[1], 3, "Explorer");
                //     doupdate();
                // }
                break;
            case 'q':
                create_user_form_active = false;
                curs_set(0);
                unpost_form(new_file_form);
                free_form(new_file_form);
                free_field(field[0]);
                // menu_driver(ctx->explorer_menu, REQ_NEXT_ITEM);
                // ctx->course_windows[1] =
                // create_explorer_window(ctx->file_tree);
                // focus_window(&ctx->course_windows[1], 3, "Explorer");
                doupdate();
                break;
            default:
                form_driver(new_file_form, ctx->key);
                wrefresh(form_window);
                break;
        }
    }
}
