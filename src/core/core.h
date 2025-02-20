#include <ncurses.h>
#include <menu.h>
#include "../data/data_access_layer.h"

void input_handler(WINDOW **windows, int *active_win, MENU **start_menu,
                      COURSE courses[], sqlite3 **db);
void handle_course_input(WINDOW **windows, int *active_win, MENU **start_menu, sqlite3 *db);
void handle_editor_input(int ch, WINDOW **edit_window, int y, int x, bool *editor_mode);
char *wrap_text(char *text, int width);
void draw_border(WINDOW *win, int color_pair, int active_window);
void focus_window(WINDOW **windows, int window, int color_pair,
                  int *active_window, char *label);
void trim(char *str);
int get_length(char *str);

