#define _POSIX_C_SOURCE 200112L
#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../models/models.h"

#include <ctype.h>
#include <fcntl.h>
#include <ncurses.h>
#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void create_pseudo_terminal(APP_CONTEXT *ctx)
{
    int ret, master;

    ret = forkpty(&master, NULL, NULL, NULL);

    disable_echo(master);

    if (ret == -1)
        perror("forkpty failed");

    if (ret == 0)
    {
        setenv("PS1", "", 1);
        setenv("INPUTRC", "/dev/null", 1);
        execlp("sh", "sh", "--norc", "--noprofile", "-i", NULL);
        perror("execlp");
        _exit(1);
    }

    fcntl(master, F_SETFL, O_NONBLOCK);

    napms(200);

    ctx->shell->masterfd = master;
}

void read_term_input_and_write_to_pty(APP_CONTEXT *ctx)
{
    if (ctx->key != '\n')
    {
        ctx->shell->buf[ctx->shell->curr_buf_idx] = ctx->key;
        mvwprintw(ctx->terminal_window, 1, ctx->shell->curr_buf_idx + 3, "%c",
                  ctx->key);
        wrefresh(ctx->terminal_window);
        ctx->shell->curr_buf_idx += 1;
    }
    else
    {
        ctx->shell->buf[ctx->shell->curr_buf_idx] = '\n';
        ctx->shell->buf[ctx->shell->curr_buf_idx + 1] = '\0';
        write(ctx->shell->masterfd, ctx->shell->buf,
              ctx->shell->curr_buf_idx + 2);
        memset(ctx->shell->buf, 0, BUFSIZ);

        napms(200);

        read_term_output_and_print_in_term(ctx);
    }
}

void read_term_output_and_print_in_term(APP_CONTEXT *ctx)
{
    int ret;
    int clean_len = 0;
    char buf[BUFSIZ];

    mvwprintw(ctx->terminal_window, 6, 1, "%s", "in read_term_output");
    wrefresh(ctx->terminal_window);

    ret = read(ctx->shell->masterfd, buf, BUFSIZ);
    if (ret == -1)
    {
        mvwprintw(ctx->shell->term_inner_win, 8, 1, "%s", "error");
        wrefresh(ctx->shell->term_inner_win);
    }

    if (ret > 0)
    {
        clean_len = strip_ansi_escape_codes(buf, ret);
        buf[clean_len] = '\0';
    }

    int i = 0;
    int j = 1;
    int k = 0;
    bool prec_space_remaining = true;

    while (prec_space_remaining)
    {
        if (isspace(buf[i]))
        {
            i++;
        }
        else if (!isspace(buf[i]))
        {
            prec_space_remaining = false;
        }
    }

    append_term_ouput_to_buf(buf, ret, ctx->shell->term_buffer);

    while (i < clean_len)
    {
        if (k > 99)
        {
            j++;
            k = 0;
        }
        mvwaddch(ctx->shell->term_inner_win, j, k, buf[i]);
        i++;
        k++;
    }

    wrefresh(ctx->shell->term_inner_win);
}

void disable_echo(int fd)
{
    struct termios t;
    if (tcgetattr(fd, &t) == -1)
        return;
    t.c_lflag &= ~(ECHO | ECHONL); // disable echo and newline echo
    tcsetattr(fd, TCSANOW, &t);
}

void append_term_ouput_to_buf(char buf[], int buf_len, TEXT_BUFFER *term_buf)
{
    int rem_bytes = 100;
    LINE *curr_line = initialize_line();

    if (term_buf->num_of_lines == 0)
    {
        curr_line = term_buf->first_line;
    }
    else {
        curr_line = term_buf->current_line;
    }

    if (buf_len >= 99)
    {
        while (rem_bytes > 0)
        {
            strncpy(curr_line->buf_, buf, rem_bytes);
            rem_bytes = buf_len % 100;
            curr_line->next = initialize_line();
            curr_line = curr_line->next;
            term_buf->current_line = curr_line;
        }
    }
    else {
        strncpy(term_buf->first_line->buf_, buf, buf_len);
        curr_line->next = initialize_line();
        curr_line = curr_line->next;
        term_buf->current_line = curr_line;
    }
}

void print_term_buf(WINDOW *term_win, TEXT_BUFFER *term_buf)
{
    
}
