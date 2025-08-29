#include "../../ntutor.h"
#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../models/models.h"

#include <curses.h>
#include <fcntl.h>
#include <ncurses.h>
#include <pty.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void move_cursor_left(APP_CONTEXT *ctx)
{
    if (ctx->shell->curr_buf_idx >= 1)
    {
        int nol = ctx->shell->term_buffer->num_of_lines;
        ctx->shell->term_buffer->current_col--;
        ctx->shell->curr_buf_idx--;

        wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
              ctx->shell->term_buffer->current_col);
        wrefresh(ctx->shell->term_inner_win);
    }
}

void move_cursor_right(APP_CONTEXT *ctx)
{
    if (ctx->shell->curr_buf_idx < strlen(ctx->shell->buf))
    {
        int nol = ctx->shell->term_buffer->num_of_lines;
        ctx->shell->term_buffer->current_col++;
        ctx->shell->curr_buf_idx++;

        wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
              ctx->shell->term_buffer->current_col);
        wrefresh(ctx->shell->term_inner_win);
    }
}

void delete_char_with_back_space(APP_CONTEXT *ctx)
{
    if (ctx->shell->curr_buf_idx > 0)
    {
        int x = ctx->shell->curr_buf_idx;
        int nol = ctx->shell->term_buffer->num_of_lines;
        for (int i = 0; i < strlen(ctx->shell->buf); i++)
        {
            mvwaddch(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7, i + 2,
                     ' ');
        }
        // mvwprintw(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7, 2, "%s",
        //           "                   ");
        memmove(&ctx->shell->buf[x - 1], &ctx->shell->buf[x],
                strlen(ctx->shell->buf) - x + 1);
        memset(&ctx->shell->buf[strlen(ctx->shell->buf)], '\0', 1);
        mvwprintw(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7, 2, "%s",
                  ctx->shell->buf);
        ctx->shell->curr_buf_idx--;
        ctx->shell->term_buffer->current_col--;
        wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7, x + 1);
        wrefresh(ctx->shell->term_inner_win);
    }
}

void scroll_up(APP_CONTEXT *ctx)
{
    if (ctx->shell->term_buffer->scroll_offset <
        ctx->shell->term_buffer->num_of_lines - 8)
    {
        ctx->shell->term_buffer->scroll_offset++;
        print_term_buf(ctx->shell->term_inner_win, ctx->shell->term_buffer);
        wrefresh(ctx->shell->term_inner_win);
    }
}

void scroll_down(APP_CONTEXT *ctx)
{
    if (ctx->shell->term_buffer->scroll_offset > 0)
    {
        werase(ctx->shell->term_inner_win);
        ctx->shell->term_buffer->scroll_offset--;
        print_term_buf(ctx->shell->term_inner_win, ctx->shell->term_buffer);
        wrefresh(ctx->shell->term_inner_win);
    }
}

void read_term_input_and_write_to_pty(APP_CONTEXT *ctx)
{
    int nol = ctx->shell->term_buffer->num_of_lines;
    int x = ctx->shell->curr_buf_idx;

    memmove(&ctx->shell->buf[x + 2], &ctx->shell->buf[x + 1],
            strlen(ctx->shell->buf) - x);
    ctx->shell->buf[x] = ctx->key;
    ctx->shell->term_buffer->current_col++;

    if (ctx->shell->term_buffer->scroll_offset > 0)
    {
        ctx->shell->term_buffer->scroll_offset = 0;
        werase(ctx->shell->term_inner_win);
        print_term_buf(ctx->shell->term_inner_win, ctx->shell->term_buffer);
    }

    mvwprintw(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7, 2, "%s",
              ctx->shell->buf);

    wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
          ctx->shell->term_buffer->current_col);

    ctx->shell->curr_buf_idx += 1;

    wnoutrefresh(ctx->edit_window);
    wnoutrefresh(ctx->shell->term_inner_win);
    doupdate();
}

void check_for_cd_cmd(APP_CONTEXT *ctx)
{
    char delim[] = " ";
    char *shell_buf = strdup(ctx->shell->buf);

    char *first_cmd_part = strsep(&shell_buf, delim);

    mvwprintw(ctx->edit_window, 20, 60, "%s", "from cd func");

    if (strcmp(first_cmd_part, "cd") == 0)
    {
        ctx->shell->cwd = strdup(strsep(&shell_buf, delim));
        chdir(ctx->shell->cwd);
        mvwprintw(ctx->edit_window, 24, 60, "%s", ctx->shell->cwd);
        mvwprintw(ctx->edit_window, 25, 60, "%lu", strlen(ctx->shell->cwd));
    }
}

void submit_command(APP_CONTEXT *ctx)
{
    werase(ctx->shell->term_inner_win);

    LINE *curr_line = initialize_line();
    ctx->shell->term_buffer->current_line->next = curr_line;
    curr_line->prev = ctx->shell->term_buffer->current_line;
    ctx->shell->term_buffer->current_line = curr_line;
    ctx->shell->term_buffer->current_line->next = NULL;
    ctx->shell->term_buffer->num_of_lines++;

    char *command = strdup(ctx->shell->buf);
    strncat(command, " 2>&1", 5);
    command[ctx->shell->curr_buf_idx + 5] = '\n';
    command[ctx->shell->curr_buf_idx + 6] = '\0';
    FILE *fp = popen(command, "r");
    char buf[BUFSIZ];
    int i = 0;
    char c;
    while (fread(&c, 1, 1, fp))
    {
        buf[i] = c;
        i++;
    }
    buf[i - 1] = '\0';
    pclose(fp);

    check_for_cd_cmd(ctx);

    ctx->shell->buf[ctx->shell->curr_buf_idx] = '\n';
    ctx->shell->buf[ctx->shell->curr_buf_idx + 1] = '\0';


    memcpy(ctx->shell->term_buffer->current_line->prev->buf_ + 2,
           ctx->shell->buf, strlen(ctx->shell->buf));

    append_term_ouput_to_buf(ctx->edit_window, buf, i - 1,
                             ctx->shell->term_buffer);

    print_term_buf(ctx->shell->term_inner_win, ctx->shell->term_buffer);

    mvwprintw(ctx->edit_window, 23, 5, "buf: %s", ctx->shell->buf);

    memset(ctx->shell->buf, 0, BUFSIZ);
    ctx->shell->term_buffer->current_col = 2;
    int nol = ctx->shell->term_buffer->num_of_lines;
    mvwprintw(ctx->edit_window, 20, 5, "nol: %i", nol);
    mvwprintw(ctx->edit_window, 21, 5, "num_of_lines: %i",
              ctx->shell->term_buffer->num_of_lines);

    wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
          ctx->shell->term_buffer->current_col);

    ctx->shell->curr_buf_idx = 0;

    wnoutrefresh(ctx->edit_window);
    wnoutrefresh(ctx->shell->term_inner_win);
    doupdate();
}

void append_term_ouput_to_buf(WINDOW *win, char *buf, int buf_len,
                              TEXT_BUFFER *term_buf)
{
    int j = 0;
    int k = 0;
    int line_number = 0;

    LINE *curr_line = initialize_line();

    curr_line = term_buf->current_line;

    while (j < buf_len + 1)
    {
        if (j > 1 && buf[j] == '\0')
        {
            curr_line->buf_[k] = '\0';
            curr_line->line_num = line_number;
            term_buf->num_of_lines++;
            term_buf->current_line->next = curr_line;
            curr_line->prev = term_buf->current_line;
            term_buf->current_line = curr_line;

            curr_line = initialize_line();
            term_buf->current_line->next = curr_line;
            curr_line->prev = term_buf->current_line;
            term_buf->current_line = curr_line;
            term_buf->current_line->next = NULL;
            break;
        }
        else if (buf[j] == '\n')
        {
            curr_line->buf_[k] = '\n';
            curr_line->buf_[k + 1] = '\0';
            k = 0;
            j++;
            term_buf->num_of_lines++;
            term_buf->current_line->next = curr_line;
            curr_line->prev = term_buf->current_line;
            term_buf->current_line = curr_line;
            curr_line = initialize_line();
            continue;
        }
        else if (k > EDITOR_WIDTH)
        {
            k = 0;
            term_buf->current_line->next = curr_line;
            curr_line->prev = term_buf->current_line;
            term_buf->current_line = curr_line;
            term_buf->current_line->next = NULL;
            term_buf->num_of_lines++;
            curr_line = initialize_line();
            continue;
        }

        curr_line->buf_[k] = buf[j];
        curr_line->length++;
        j++;
        k++;
    }

    mvwprintw(win, 9, 3, "%s", term_buf->current_line->buf_);
    memcpy(term_buf->current_line->buf_, "> ", 2);
}

void print_term_buf(WINDOW *term_win, TEXT_BUFFER *term_buf)
{
    unsigned int nol = term_buf->num_of_lines;
    unsigned int start_line = nol < 8 ? 0 : nol - 8;

    start_line -= term_buf->scroll_offset;

    LINE *curr_line = term_buf->first_line;

    for (int i = 0; i < start_line; i++)
    {
        curr_line = curr_line->next;
    }

    int i = 0;
    while (curr_line && i < 8)
    {
        mvwprintw(term_win, i, 0, "%s", curr_line->buf_);
        curr_line = curr_line->next;
        i++;
    }
}
