#include <ncurses.h>
#include <menu.h>
#include "../data/data_access_layer.h"

void input_handler(WINDOW **windows, int *active_win, MENU **start_menu,
                      COURSE courses[], sqlite3 **db);
char *wrap_text(char *text, int width);

