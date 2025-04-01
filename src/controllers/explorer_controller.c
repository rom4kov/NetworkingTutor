#include <stdlib.h>
#define _DEFAULT_SOURCE

// #include <ctype.h>
#include "../core/core.h"
#include "../data/data_access_layer.h"
#include "../models/models.h"
#include "../views/views.h"

#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void handle_explorer_input(APP_CONTEXT *ctx)
{
    ITEM *curr_item;
    bool new_file_form_active = false;
    bool del_file_form_active = false;
    WINDOW *inner_win = derwin(ctx->course_windows[1], 3, 18, 2, 2);
    WINDOW *form_window = derwin(inner_win, 1, 16, 1, 1);
    FORM *new_file_form = NULL;
    FIELD *field[2];
    // ctx->file_tree->current_entry = ctx->file_tree->first_entry;

    switch (ctx->key)
    {
        case KEY_DOWN:
            if (ctx->file_tree->curr_entry_nr <
                ctx->file_tree->num_of_entries - 2)
            {
                ctx->file_tree->current_entry = ctx->file_tree->current_entry->next;
                mvwprintw(ctx->course_windows[0], 2, 24, "                            ");
                mvwprintw(ctx->course_windows[0], 2, 24, "%s", ctx->file_tree->current_entry->path);
                mvwprintw(ctx->course_windows[0], 2, 34, "%i", ctx->file_tree->curr_entry_nr);
                wrefresh(ctx->course_windows[0]);
                ctx->file_tree->curr_entry_nr++;
                ctx->course_windows[1] = create_explorer_window(
                    ctx->file_tree, &ctx->course_windows[2], ctx->file_tree->curr_entry_nr);
                wrefresh(ctx->course_windows[1]);
            }
            break;
        case KEY_UP:
            if (ctx->file_tree->curr_entry_nr > 0)
            {
                ctx->file_tree->current_entry = ctx->file_tree->current_entry->prev;
                mvwprintw(ctx->course_windows[0], 2, 24, "                            ");
                mvwprintw(ctx->course_windows[0], 2, 24, "%s", ctx->file_tree->current_entry->path);
                mvwprintw(ctx->course_windows[0], 2, 34, "%i", ctx->file_tree->curr_entry_nr);
                wrefresh(ctx->course_windows[0]);
                ctx->file_tree->curr_entry_nr--;
                ctx->course_windows[1] = create_explorer_window(
                    ctx->file_tree, &ctx->course_windows[2], ctx->file_tree->curr_entry_nr);
                wrefresh(ctx->course_windows[1]);
            }
            break;
        case 10:
            curr_item = current_item(ctx->explorer_menu);
            ctx->filename = (char *)item_name(curr_item);
            // trim(&ctx->filename);
            // mvwprintw(ctx->course_windows[0], 2, 0, "%s", ctx->filename);
            // wrefresh(ctx->course_windows[0]);

            // char *curr_name = calloc(30, sizeof(char));
            char *curr_path = calloc(30, sizeof(char));
            //     return_trimmed(ctx->file_tree->current_entry->name);
            // while (ctx->file_tree->current_entry != NULL)
            // {
            // for (int i = 0; i < 9; i++)
            //     curr_name[i] = '\0';
            // memset(curr_name, 0, strlen(curr_name));
            // curr_name = return_trimmed(ctx->file_tree->current_entry->name);
            curr_path = return_trimmed(ctx->file_tree->current_entry->path);
            mvwprintw(ctx->course_windows[0], 2, 24, "%s", ctx->file_tree->current_entry->path);
            wrefresh(ctx->course_windows[0]);
            // curr_name[strlen(curr_name)] = '\0';
            // mvwprintw(ctx->course_windows[0], 2, 12, "%s", curr_name);
            // mvwprintw(ctx->course_windows[0], 2, 22, "%s", curr_path);
            // wrefresh(ctx->course_windows[0]);
            // if (strcmp(ctx->filename, curr_name) == 0)
            // {
            if (ctx->file_tree->current_entry->type == 4)
            {
                if (ctx->file_tree->current_entry->state == 'c')
                {
                    ctx->file_tree->current_entry->state = 'o';
                    open_sub_directory(curr_path, ctx->file_tree,
                                       &ctx->course_windows[2]);
                }
                else if (ctx->file_tree->current_entry->state == 'o')
                {
                    ctx->file_tree->current_entry->state = 'c';
                    close_sub_directory(
                        ctx->file_tree->current_entry,
                        ctx->file_tree->current_entry->num_of_entries,
                        ctx->file_tree, &ctx->course_windows[2]);
                }
                wclear(ctx->course_windows[1]);
                // int i_idx = item_index(curr_item);
                ctx->course_windows[1] = create_explorer_window(
                    ctx->file_tree, &ctx->course_windows[2], ctx->file_tree->curr_entry_nr);
                break;
            }
            else
            {
                deallocate_buffer(ctx->t_buffer);
                ctx->t_buffer = initialize_buffer();
                if (ctx->file && ctx->file->_fileno > 0)
                    fclose(ctx->file);
                open_file(ctx);

                new_file_form_active = false;
                ctx->explorer_mode = false;
                ctx->editor_mode = true;
                ctx->active_window = 2;
                focus_window(&ctx->course_windows[1], 2, "Explorer");
                focus_window(&ctx->course_windows[2], 3, "Editor");
                curs_set(2);
                wmove(ctx->edit_window, 0, 0);
                wnoutrefresh(ctx->course_windows[1]);
                wnoutrefresh(ctx->line_num_win);
                wnoutrefresh(ctx->course_windows[2]);
                wnoutrefresh(ctx->edit_window);
                doupdate();
                break;
            }
            wrefresh(ctx->course_windows[1]);
            break;
            // }
            // ctx->file_tree->current_entry =
            //     ctx->file_tree->current_entry->next;
            // // }
            // break;
        case 'a':
            create_new_file_input(&inner_win, &form_window, &new_file_form,
                                  field, "Create file");

            new_file_form_active = true;

            while (new_file_form_active)
            {
                ctx->key = getch();

                switch (ctx->key)
                {
                    case 263: // Backspace
                        form_driver(new_file_form, REQ_VALIDATION);
                        FIELD *current = current_field(new_file_form);
                        char *buf = field_buffer(current, 0);
                        trim(&buf);
                        if (buf && get_length(buf) > 0)
                        {
                            form_driver(new_file_form, REQ_DEL_PREV);
                            wrefresh(form_window);
                        }
                        break;
                    case 10:
                        form_driver(new_file_form, REQ_VALIDATION);
                        ctx->filename = field_buffer(field[0], 0);
                        trim(&ctx->filename);
                        new_file_form_active = false;
                        ctx->explorer_mode = false;
                        ctx->editor_mode = true;
                        ctx->active_window = 2;
                        focus_window(&ctx->course_windows[2], 3, "Editor");

                        deallocate_buffer(ctx->t_buffer);
                        ctx->t_buffer = initialize_buffer();
                        if (ctx->file)
                            fclose(ctx->file);
                        open_new_file(ctx);

                        ctx->course_windows[1] = create_explorer_window(
                            ctx->file_tree, &ctx->course_windows[2], 0);
                        wmove(ctx->edit_window, 0, 0);
                        // mvwprintw(ctx->course_windows[1], 28, 2, "File: %s",
                        //           ctx->filename);
                        wrefresh(ctx->course_windows[1]);
                        wrefresh(ctx->edit_window);
                        break;
                    case 'q':
                        new_file_form_active = false;
                        curs_set(0);
                        unpost_form(new_file_form);
                        free_form(new_file_form);
                        free_field(field[0]);
                        menu_driver(ctx->explorer_menu, REQ_NEXT_ITEM);
                        ctx->course_windows[1] = create_explorer_window(
                            ctx->file_tree, &ctx->course_windows[2], 0);
                        focus_window(&ctx->course_windows[1], 3, "Explorer");
                        break;
                    default:
                        form_driver(new_file_form, ctx->key);
                        wrefresh(form_window);
                        break;
                }
            }

            break;
        case 'r':
            break;
        case 'd':
            create_new_file_input(&inner_win, &form_window, &new_file_form,
                                  field, "Delete file");
            del_file_form_active = true;

            while (del_file_form_active)
            {
                ctx->key = getch();

                switch (ctx->key)
                {
                    case 263: // Backspace
                        form_driver(new_file_form, REQ_VALIDATION);
                        FIELD *current = current_field(new_file_form);
                        char *buf = field_buffer(current, 0);
                        trim(&buf);
                        if (buf && get_length(buf) > 0)
                        {
                            form_driver(new_file_form, REQ_DEL_PREV);
                            wrefresh(form_window);
                        }
                        break;
                    case 10:
                        form_driver(new_file_form, REQ_VALIDATION);
                        char *new_file_name = field_buffer(field[0], 0);
                        trim(&new_file_name);
                        del_file_form_active = false;
                        ctx->explorer_mode = true;
                        ctx->editor_mode = false;
                        ctx->active_window = 1;

                        if (strcmp(new_file_name, ctx->filename) == 0)
                        {
                            wclear(ctx->line_num_win);
                            wclear(ctx->course_windows[2]);
                            wclear(ctx->edit_window);
                            deallocate_buffer(ctx->t_buffer);
                            ctx->t_buffer = initialize_buffer();
                        }

                        if (ctx->file)
                            fclose(ctx->file);
                        remove(new_file_name);

                        ctx->course_windows[1] = create_explorer_window(
                            ctx->file_tree, &ctx->course_windows[2], 0);

                        curs_set(0);
                        // mvwprintw(ctx->course_windows[1], 30, 2, "old: %s",
                        //           ctx->filename);
                        // mvwprintw(ctx->course_windows[1], 31, 2, "new: %s",
                        //           new_file_name);
                        focus_window(&ctx->course_windows[2], 2, "Editor");
                        focus_window(&ctx->course_windows[1], 3, "Explorer");
                        wnoutrefresh(ctx->course_windows[1]);
                        wnoutrefresh(ctx->line_num_win);
                        wnoutrefresh(ctx->course_windows[2]);
                        wnoutrefresh(ctx->edit_window);
                        doupdate();
                        break;
                    case 'q':
                        del_file_form_active = false;
                        curs_set(0);
                        unpost_form(new_file_form);
                        free_form(new_file_form);
                        free_field(field[0]);
                        menu_driver(ctx->explorer_menu, REQ_NEXT_ITEM);
                        ctx->course_windows[1] = create_explorer_window(
                            ctx->file_tree, &ctx->course_windows[2], 0);
                        focus_window(&ctx->course_windows[1], 3, "Explorer");
                        break;
                    default:
                        form_driver(new_file_form, ctx->key);
                        wrefresh(form_window);
                        break;
                }
            }
            break;
        case KEY_F(1):
            wrefresh(ctx->course_windows[1]);
            ctx->explorer_mode = false;
            break;
    }
}
