#include "../models/models.h"
#include <ncurses.h>
#include <menu.h>

void handle_greeter_input(APP_CONTEXT *ctx)
{
    switch (ctx->key) {
        case KEY_DOWN:
            menu_driver(ctx->greeter_menu, REQ_DOWN_ITEM);
            wrefresh(ctx->greeter_screen);
            break;
        case KEY_UP:
            menu_driver(ctx->greeter_menu, REQ_UP_ITEM);
            wrefresh(ctx->greeter_screen);
            break;
    }
}
