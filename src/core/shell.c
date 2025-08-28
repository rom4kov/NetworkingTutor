#define _POSIX_C_SOURCE 200809L
#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../models/models.h"

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

void read_term_input_and_write_to_pty(APP_CONTEXT *ctx)
{
    int nol = ctx->shell->term_buffer->num_of_lines;
    int ccol = ctx->shell->term_buffer->current_col;
    int x = ctx->shell->curr_buf_idx;

    memmove(&ctx->shell->buf[x + 1], &ctx->shell->buf[x],
            strlen(ctx->shell->buf) - x);
    ctx->shell->buf[x] = ctx->key;
    ctx->shell->term_buffer->current_col++;

    // ctx->shell->buf[ctx->shell->curr_buf_idx] = ctx->key;
    // ctx->shell->term_buffer->current_line
    //     ->buf_[ctx->shell->term_buffer->current_col] = ctx->key;

    mvwprintw(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7, ccol, "%s",
              ctx->shell->buf);

    wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
          ctx->shell->term_buffer->current_col);

    ctx->shell->curr_buf_idx += 1;

    wnoutrefresh(ctx->edit_window);
    wnoutrefresh(ctx->shell->term_inner_win);
    doupdate();
}

void submit_command(APP_CONTEXT *ctx)
{
    wclear(ctx->shell->term_inner_win);
    print_term_buf(ctx->edit_window, ctx->shell->term_buffer);
    ctx->shell->buf[ctx->shell->curr_buf_idx] = '\n';
    ctx->shell->buf[ctx->shell->curr_buf_idx + 1] = '\0';

    memcpy(ctx->shell->term_buffer->current_line->buf_ + 2, ctx->shell->buf,
           strlen(ctx->shell->buf));

    LINE *curr_line = initialize_line();
    ctx->shell->term_buffer->current_line->next = curr_line;
    curr_line->prev = ctx->shell->term_buffer->current_line;
    ctx->shell->term_buffer->current_line = curr_line;
    ctx->shell->term_buffer->current_line->next = NULL;
    ctx->shell->term_buffer->num_of_lines++;

    FILE *fp = popen(ctx->shell->buf, "r");
    char buf[BUFSIZ];
    int i = 0;
    char c;
    while (fread(&c, 1, 1, fp))
    {
        buf[i] = c;
        i++;
    }
    buf[i - 1] = '\0';
    append_term_ouput_to_buf(ctx->edit_window, buf, i - 1,
                             ctx->shell->term_buffer);
    // mvwprintw(ctx->edit_window, 15, 3, "%s", buf);
    print_term_buf(ctx->shell->term_inner_win, ctx->shell->term_buffer);
    // write(ctx->shell->masterfd, ctx->shell->buf,
    //       ctx->shell->curr_buf_idx + 2);

    memset(ctx->shell->buf, 0, BUFSIZ);
    ctx->shell->term_buffer->current_col = 2;
    int nol = ctx->shell->term_buffer->num_of_lines;
    mvwprintw(ctx->edit_window, 20, 5, "nol: %i", nol);
    mvwprintw(ctx->edit_window, 21, 5, "num_of_lines: %i",
              ctx->shell->term_buffer->num_of_lines);

    // int nol = ctx->shell->term_buffer->num_of_lines;
    wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
          ctx->shell->term_buffer->current_col);

    ctx->shell->curr_buf_idx = 0;

    // napms(200);

    wnoutrefresh(ctx->edit_window);
    wnoutrefresh(ctx->shell->term_inner_win);
    doupdate();
}

void disable_echo(int fd)
{
    struct termios t;
    if (tcgetattr(fd, &t) == -1)
        return;
    t.c_lflag &= ~(ECHO | ECHONL); // disable echo and newline echo
    tcsetattr(fd, TCSANOW, &t);
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
            // curr_line->buf_[0] = '>';
            // curr_line->buf_[1] = ' ';
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
        else if (k > 90)
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
    // mvwprintw(win, 3, 3, "%s", buf);
    // mvwprintw(win, 5, 3, "%s", term_buf->first_line->buf_);
    // mvwprintw(win, 6, 3, "%s", term_buf->first_line->next->buf_);
    // mvwprintw(win, 7, 3, "%s", term_buf->first_line->next->next->buf_);
    // mvwprintw(win, 8, 3, "%s", term_buf->first_line->next->next->next->buf_);
    mvwprintw(win, 9, 3, "%s", term_buf->current_line->buf_);
    memcpy(term_buf->current_line->buf_, "> ", 2);
}

void print_term_buf(WINDOW *term_win, TEXT_BUFFER *term_buf)
{
    // wclear(term_win);
    unsigned int nol = term_buf->num_of_lines;
    unsigned int start_line = nol < 8 ? 0 : nol - 8;

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

void read_term_output_and_print_in_term(APP_CONTEXT *ctx)
{
    // int ret;
    // int clean_len = 0;
    // char buf[BUFSIZ];
    // char *trimmed_buf = NULL;
    //
    // // mvwprintw(ctx->terminal_window, 6, 1, "%s", "in read_term_output");
    // // wrefresh(ctx->terminal_window);
    //
    // ret = read(ctx->shell->masterfd, buf, BUFSIZ);
    // if (ret == -1)
    // {
    //     mvwprintw(ctx->shell->term_inner_win, 8, 1, "%s", "error");
    //     wrefresh(ctx->shell->term_inner_win);
    // }
    //
    // if (ret > 0)
    // {
    //     clean_len = strip_ansi_escape_codes(buf, ret);
    //     buf[clean_len - 1] = '\0';
    //     // char *p = buf;
    //     trimmed_buf = return_trimmed(buf);
    //     // trim(&p);
    // }

    // int i = 0;
    // int j = 1;
    // int k = 0;
    // bool prec_space_remaining = true;
    //
    // while (prec_space_remaining)
    // {
    //     if (isspace(buf[i]))
    //     {
    //         i++;
    //     }
    //     else if (!isspace(buf[i]))
    //     {
    //         prec_space_remaining = false;
    //     }
    // }

    // append_term_ouput_to_buf(ctx->edit_window, trimmed_buf, clean_len,
    // ctx->shell->term_buffer); mvwprintw(ctx->shell->term_inner_win, 5, 0,
    // "%s", ctx->shell->term_buffer->first_line->buf_);
    // mvwprintw(ctx->shell->term_inner_win, 6, 0, "%s",
    // ctx->shell->term_buffer->first_line->next->buf_);

    print_term_buf(ctx->shell->term_inner_win, ctx->shell->term_buffer);

    // mvwprintw(ctx->shell->term_inner_win, 6, 0, "%s",
    //           ctx->shell->term_buffer->first_line->next->buf_);
    // mvwprintw(ctx->shell->term_inner_win, 5, 0, "%s",
    //           ctx->shell->term_buffer->first_line->buf_);
    // mvwprintw(ctx->shell->term_inner_win, 5, 0, "%i", clean_len);
    // mvwprintw(ctx->shell->term_inner_win, 5, 0, "%c", buf[i]);

    // while (i < clean_len)
    // {
    //     if (k > 99)
    //     {
    //         j++;
    //         k = 0;
    //     }
    //     mvwaddch(ctx->shell->term_inner_win, j, k, buf[i]);
    //     i++;
    //     k++;
    // }

    wnoutrefresh(ctx->edit_window);
    wnoutrefresh(ctx->shell->term_inner_win);
    doupdate();
}

// void create_pseudo_terminal(APP_CONTEXT *ctx)
// {
//     int ret, master;
//
//     ret = forkpty(&master, NULL, NULL, NULL);
//
//     disable_echo(master);
//
//     if (ret == -1)
//         perror("forkpty failed");
//
//     if (ret == 0)
//     {
//         setenv("PS1", "", 1);
//         setenv("INPUTRC", "/dev/null", 1);
//         execlp("sh", "sh", "--norc", "--noprofile", "-i", NULL);
//         perror("execlp");
//         _exit(1);
//     }
//
//     fcntl(master, F_SETFL, O_NONBLOCK);
//
//     napms(200);
//
//     ctx->shell->masterfd = master;
// }
