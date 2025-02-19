#include <form.h>
#include <curses.h>
#include <ncurses.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void create_user_form(WINDOW **window, FORM **user_form, FIELD **fields)
{
    // FIELD *fields[3];
    int rows, cols;
    WINDOW *inner_win = derwin(*window, LINES - 2, COLS - (WU * 7 + 4) - 2, 1, 1);
    wclear(*window);

    fields[0] = new_field(1, 20, 0, 1, 0, 0);
    fields[1] = new_field(1, 20, 2, 1, 0, 0);
    fields[2] = NULL;

    // set_field_buffer(fields[0], 10, "hello1");
    set_field_back(fields[0], A_UNDERLINE);
    field_opts_off(fields[0], O_AUTOSKIP);

    // set_field_buffer(fields[1], 10, "hello");
    set_field_back(fields[1], A_UNDERLINE);
    field_opts_off(fields[1], O_AUTOSKIP);

    *user_form = new_form(fields);

    scale_form(*user_form, &rows, &cols);

    WINDOW *form_win = derwin(inner_win, rows, cols, 3, 12);

    wattron(inner_win, A_BOLD);
    mvwprintw(inner_win, 1, 2, "ACCOUNT");
    wattroff(inner_win, A_BOLD);
    mvwprintw(inner_win, 3, 2, "Your name: ");
    mvwprintw(inner_win, 5, 2, "Language:  ");
    mvwprintw(inner_win, 27, 2, "LINES: %i", LINES);
    mvwprintw(inner_win, 28, 2, "COLS: %i", COLS);

    set_form_win(*user_form, inner_win);
    set_form_sub(*user_form, form_win);

    curs_set(1);
    set_current_field(*user_form, fields[0]);
    post_form(*user_form);

    // wrefresh(*window);
    // mvwprintw(inner_win, 38, 2, "Inner win string");
    wrefresh(inner_win);
}
