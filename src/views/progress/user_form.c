#include "../../core/core.h"
#include <form.h>
#include <ncurses.h>

#define WU COLS / 12 // WU for WIDTH_UNIT

void create_user_form(WINDOW *window, FORM **user_form, FIELD **fields)
{
    int rows, cols;
    // rows = 10;
    // cols = 15;
    WINDOW *inner_win = derwin(window, 5, 40, 8, 8);
    // wclear(window);

    fields[0] = new_field(1, 20, 0, 1, 0, 0);
    fields[1] = NULL;

    // set_field_buffer(fields[0], 10, "hello1");
    set_field_back(fields[0], A_UNDERLINE);
    field_opts_off(fields[0], O_AUTOSKIP);

    *user_form = new_form(fields);

    scale_form(*user_form, &rows, &cols);

    WINDOW *form_win = derwin(inner_win, rows, cols, 2, 14);

    mvwprintw(inner_win, 2, 0, "Your name: ");

    set_form_win(*user_form, inner_win);
    set_form_sub(*user_form, form_win);

    set_current_field(*user_form, fields[0]);
    post_form(*user_form);
    
    wnoutrefresh(inner_win);
    wnoutrefresh(form_win);
}
