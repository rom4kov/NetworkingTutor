#include "../views/views.h"
#include "../views/user_form.h"
#include <curses.h>
#include <form.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>

#define WINDOW_COUNT 6
#define WU COLS / 12 // WU for WIDTH_UNIT
#define CARD_WIDTH (((WU * 7) / 3) + 1)

void input_handler(WINDOW **windows, int *active_win, MENU **start_menu,
                   COURSE courses[], sqlite3 **db)
{
    bool on_main_menu = true;
    FORM *user_form = NULL;
    FIELD *fields[3];
    ITEM *curr_item;
    int this_win;
    int ch;
    while (on_main_menu)
    {
        ch = getch();
        ESCDELAY = 100;

        if (ch == 27) return;

        if (*active_win == 0)
        {
            switch (ch)
            {
                case KEY_LEFT:
                    menu_driver(*start_menu, REQ_PREV_ITEM);
                    wrefresh(windows[0]);
                    break;
                case KEY_RIGHT:
                    menu_driver(*start_menu, REQ_NEXT_ITEM);
                    wrefresh(windows[0]);
                    break;
                case 9:
                case KEY_DOWN:
                    *active_win = 1;
                    focus_window(windows, 0, 2, active_win, "Navigation");
                    focus_window(windows, 1, 3, active_win, "");
                    break;
                case '\n':
                    curr_item = current_item(*start_menu);
                    if (item_index(curr_item) == 2)
                    {
                        focus_window(windows, 0, 2, active_win, "Navigation");
                        focus_window(windows, 5, 3, active_win, "Details");
                        *active_win = 5;
                        mvwprintw(windows[5], 30, 2, "active window: %i",
                                  *active_win);
                        create_user_form(&windows[5], &user_form, fields);
                        focus_window(windows, 5, 3, active_win, "Details");
                        wmove(windows[5], 4, 14);
                        wrefresh(windows[5]);
                    }
            }
        }
        else if (*active_win == 1)
        {
            switch (ch)
            {
                case 9:
                case KEY_DOWN:
                    *active_win = 2;
                    draw_border(windows[1], 2, *active_win);
                    windows[2] = create_course_preview_card(
                        CARD_WIDTH * (*active_win - 2), active_win, *active_win,
                        &courses[0]);
                    break;
                case KEY_UP:
                    *active_win = 0;
                    focus_window(windows, 0, 3, active_win, "Navigation");
                    focus_window(windows, 1, 2, active_win, "");
                    break;
                case KEY_RIGHT:
                    *active_win = 5;
                    focus_window(windows, 1, 2, active_win, "");
                    focus_window(windows, 5, 3, active_win, "Details");
                    break;
            }
        }
        else if (*active_win == 2 || *active_win == 3)
        {
            switch (ch)
            {
                case 9:
                case KEY_RIGHT:
                    *active_win = (*active_win + 1) % WINDOW_COUNT;
                    windows[*active_win - 1] = create_course_preview_card(
                        CARD_WIDTH * (*active_win - 3), active_win, *active_win - 1,
                        &courses[*active_win - 3]);
                    windows[*active_win] = create_course_preview_card(
                        CARD_WIDTH * (*active_win - 2), active_win, *active_win,
                        &courses[*active_win - 2]);
                    break;
                case KEY_LEFT:
                    if (*active_win == 3)
                    {
                        *active_win = 2;
                        windows[2] = create_course_preview_card(
                            0, active_win, 2, &courses[0]);
                        windows[3] = create_course_preview_card(
                            CARD_WIDTH, active_win, 3, &courses[1]);
                    }
                    break;
                case KEY_UP:
                    this_win = *active_win;
                    *active_win = 1;
                    focus_window(windows, 1, 3, active_win, "");
                    windows[this_win] = create_course_preview_card(
                        CARD_WIDTH * (this_win - 2), active_win, this_win,
                        &courses[this_win - 2]);
                    break;
                case 10: // Enter / Return key
                    on_main_menu = false;
                    for (int i = 1; i < WINDOW_COUNT; ++i)
                    {
                        delwin(windows[i]);
                        wclear(windows[i]);
                    }
                    create_course_view(*db);
                    break;
            }
        }
        else if (*active_win == 4)
        {
            switch (ch) 
            {
                case 9:
                case KEY_RIGHT:
                    *active_win = (*active_win + 1) % WINDOW_COUNT;
                    windows[*active_win - 1] = create_course_preview_card(
                        CARD_WIDTH * (*active_win - 3), active_win, *active_win - 1,
                        &courses[2]);
                    focus_window(windows, 5, 3, active_win, "Details");
                    break;
                case KEY_LEFT:
                    *active_win = 3;
                    windows[3] = create_course_preview_card(
                        CARD_WIDTH, active_win, 3, &courses[1]);
                    windows[4] = create_course_preview_card(
                        CARD_WIDTH * 2, active_win, 4, &courses[2]);
                    break;
                case KEY_UP:
                    this_win = *active_win;
                    *active_win = 1;
                    focus_window(windows, 1, 3, active_win, "");
                    windows[this_win] = create_course_preview_card(
                        CARD_WIDTH * 2, active_win, this_win, &courses[2]);
                    break;
            }
        }
        else if (*active_win == 5 && user_form)
        {
            switch (ch)
            {
                case KEY_DOWN:
                    form_driver(user_form, REQ_DOWN_FIELD);
                    form_driver(user_form, REQ_END_LINE);
                    wrefresh(windows[5]);
                    break;
                case KEY_UP:
                    form_driver(user_form, REQ_UP_FIELD);
                    form_driver(user_form, REQ_END_LINE);
                    wrefresh(windows[5]);
                    break;
                case 9:
                    form_driver(user_form, REQ_NEXT_FIELD);
                    break;
                case 263: // Backspace
                    form_driver(user_form, REQ_VALIDATION);
                    FIELD *current = current_field(user_form);
                    char *buf = field_buffer(current, 0);
                    trim(buf);
                    if (buf && get_length(buf) > 0)
                    {
                        // form_driver(user_form, REQ_LEFT_CHAR);
                        form_driver(user_form, REQ_DEL_PREV);
                        wrefresh(windows[5]);
                    }
                    break;
                case '\n':
                    form_driver(user_form, REQ_VALIDATION);
                    char *buf1 = field_buffer(fields[0], 0);
                    char *buf2 = field_buffer(fields[1], 0);
                    mvwprintw(windows[5], 38, 2, "Buffer: %s", buf1);
                    mvwprintw(windows[5], 39, 2, "Buffer: %s", buf2);
                    update_user(*db, 1, buf1, buf2);
                    curs_set(0);
                    windows[5] = create_right_side_panel(active_win, db);
                    wrefresh(windows[5]);
                    input_handler(windows, active_win, start_menu, courses, db);
                    unpost_form(user_form);
                    free_form(user_form);
                    free_field(fields[0]);
                    free_field(fields[1]);
                    break;
                default:
                    form_driver(user_form, ch);
                    wrefresh(windows[5]);
                    break;
            }
        }
        else if (*active_win == 5)
        {
            switch (ch)
            {
                case 9:
                case KEY_LEFT:
                    *active_win = 0;
                    focus_window(windows, 5, 2, active_win, "Details");
                    focus_window(windows, 0, 3, active_win, "Navigation");
                    break;
            }
        }
    }
}
