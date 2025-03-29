#include "../models/models.h"
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>

void read_file_into_buffer(FILE *file, TEXT_BUFFER *text_buf);
void handle_editor_input(APP_CONTEXT *ctx);
void handle_explorer_input(APP_CONTEXT *ctx);
