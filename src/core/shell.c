#include "../../ntutor.h"
#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../models/models.h"
#include "../views/views.h"

#include <asm-generic/errno-base.h>
#include <curses.h>
#include <fcntl.h>
#include <ncurses.h>
#include <pthread.h>
#include <pty.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
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
    if (ctx->shell->curr_buf_idx < (int)strlen(ctx->shell->buf))
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
        int cwd_len = strlen(ctx->shell->cwd);

        for (int i = 0; i < (int)strlen(ctx->shell->buf); i++)
        {
            mvwaddch(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
                     i + (cwd_len > 0 ? cwd_len + 3 : 2), ' ');
        }
        memmove(&ctx->shell->buf[x - 1], &ctx->shell->buf[x],
                strlen(ctx->shell->buf) - x + 1);
        memset(&ctx->shell->buf[strlen(ctx->shell->buf)], '\0', 1);

        mvwprintw(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
                  2 + (cwd_len > 0 ? cwd_len + 1 : 0), "%s", ctx->shell->buf);

        ctx->shell->curr_buf_idx--;
        ctx->shell->term_buffer->current_col--;
        wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
              x + 1 + (cwd_len > 0 ? cwd_len + 1 : 0));
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

void print_term_input(APP_CONTEXT *ctx)
{
    int offset = 0;
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

    int cwd_len = strlen(ctx->shell->cwd);
    mvwprintw(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
              2 + (cwd_len > 0 ? cwd_len + 1 : 0), "%s", ctx->shell->buf);

    if (ctx->shell->executable_running)
    {
        offset = 2;
    }
    wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
          ctx->shell->term_buffer->current_col - offset);

    ctx->shell->curr_buf_idx += 1;

    wnoutrefresh(ctx->shell->term_inner_win);
    doupdate();
}

bool cmd_is_cd(APP_CONTEXT *ctx)
{
    const char delim[] = " ";

    char *shell_buf = strdup(ctx->shell->buf);
    char *orig_shell_buf = shell_buf;

    const char *first_cmd_part = strsep(&shell_buf, delim);

    if (strcmp(ctx->shell->buf, "cd") == 0)
    {
        if (ctx->shell->cwd_allocated)
            free(ctx->shell->cwd);
        ctx->shell->cwd = strdup("");
        ctx->shell->cwd_allocated = true;
        chdir(ctx->user_data->home_dir);
        ctx->shell_local_cwd = ctx->user_home_dir;
        free(orig_shell_buf);
        return false;
    }
    else if (strcmp(first_cmd_part, "cd") == 0)
    {
        if (ctx->shell->cwd_allocated)
            free(ctx->shell->cwd);
        ctx->shell->cwd = strdup(strsep(&shell_buf, delim));
        ctx->shell->cwd_allocated = true;
        free(orig_shell_buf);
        return true;
    }

    free(orig_shell_buf);

    return false;
}

char *get_cwd(void)
{
    FILE *fp = popen("pwd", "r");
    unsigned short max_path_len = 512;
    char buf[max_path_len];
    int i = 0;
    char c = ' ';
    while (fread(&c, 1, 1, fp) == 1)
    {
        if (i < max_path_len - 1)
        {
            buf[i++] = c;
        }
        else
        {
            break;
        }
    }
    buf[i - 1] = '\0';

    char *cwd = strdup(buf);

    pclose(fp);

    return cwd;
}

char *get_cwd_base_name()
{
    FILE *fp = popen("pwd | xargs basename", "r");
    unsigned short max_path_len = 512;
    char buf[max_path_len];
    int i = 0;
    char c = ' ';
    while (fread(&c, 1, 1, fp) == 1)
    {
        if (i < max_path_len - 1)
        {
            buf[i++] = c;
        }
        else
        {
            break;
        }
    }
    buf[i] = '\0';

    char *cwd_basename = strdup(buf);

    pclose(fp);

    return cwd_basename;
}

void submit_command(APP_CONTEXT *ctx)
{
    werase(ctx->shell->term_inner_win);
    int cwd_len = strlen(ctx->shell->cwd);
    FILE *out = NULL;

    LINE *curr_line = initialize_line();
    ctx->shell->term_buffer->current_line->next = curr_line;
    curr_line->prev = ctx->shell->term_buffer->current_line;
    ctx->shell->term_buffer->current_line = curr_line;
    ctx->shell->term_buffer->current_line->next = NULL;
    ctx->shell->term_buffer->num_of_lines++;

    size_t len = strlen(ctx->shell->buf);
    char *command =
        malloc(len + 1 + 5 + 2);
    if (!command)
    {
        perror("malloc");
        exit(1);
    }

    strcpy(command, ctx->shell->buf);
    strcat(command, " 2>&1");
    command[len + 5] = '\n';
    command[len + 6] = '\0';

    char buf[BUFSIZ];
    int i = 0;
    char c = ' ';
    if (command[0] == '.' && command[1] == '/')
    {
        THREAD_ARGS args = {.cmd = command,
                            .file = out,
                            .pid = malloc(sizeof(pid_t)),
                            .win = ctx->edit_window};
        pthread_t run_executable;

        pthread_create(&run_executable, NULL, popen_w_pid_ret, &args);

        pthread_join(run_executable, (void **)&args);

        ctx->shell->executable_running = true;
        ctx->shell->child_pid = *args.pid;
        ctx->shell->buf[len] = '\0';

        memcpy(ctx->shell->term_buffer->current_line->prev->buf_ + cwd_len +
                   (cwd_len > 0 ? 3 : 2),
               ctx->shell->buf, strlen(ctx->shell->buf));

        print_term_buf(ctx->shell->term_inner_win, ctx->shell->term_buffer);
        wrefresh(ctx->shell->term_inner_win);

        OUTPUT_THREAD_ARGS output_thr_args = {0};
        output_thr_args.file = args.file;
        output_thr_args.pid = malloc(sizeof(pid_t));
        output_thr_args.ctx = ctx;

        pthread_t check_for_output;
        pthread_create(&check_for_output, NULL, check_running_proc_for_output,
                       &output_thr_args);

        pthread_detach(check_for_output);
    }
    else
    {
        FILE *fp = popen(command, "r");
        free(command);

        while (fread(&c, 1, 1, fp) == 1)
        {
            if (i < BUFSIZ - 1)
            {
                buf[i++] = c;
            }
            else
            {
                break;
            }
        }
        if (i > 0)
            buf[i - 1] = '\0';
        else
            buf[i] = '\0';

        pclose(fp);

        ctx->shell->buf[ctx->shell->curr_buf_idx] = '\0';

        if (cmd_is_cd(ctx))
        {
            chdir(ctx->shell->cwd);
            char *cwd = get_cwd();

            free(ctx->shell_local_cwd);
            ctx->shell_local_cwd = strdup(cwd);

            if (strcmp(cwd, ctx->user_data->home_dir) == 0)
            {
                if (ctx->shell->cwd_allocated)
                    free(ctx->shell->cwd);
                ctx->shell->cwd = strdup("");
            }
            else
            {
                if (ctx->shell->cwd_allocated)
                    free(ctx->shell->cwd);
                ctx->shell->cwd = get_cwd_base_name();
                ctx->shell->cwd[strlen(ctx->shell->cwd) - 1] = '\0';
            }
            free(cwd);
        }

        ctx->shell->buf[ctx->shell->curr_buf_idx] = '\n';
        ctx->shell->buf[ctx->shell->curr_buf_idx + 1] = '\0';

        memcpy(ctx->shell->term_buffer->current_line->prev->buf_ + cwd_len +
                   (cwd_len > 0 ? 3 : 2),
               ctx->shell->buf, strlen(ctx->shell->buf));

        append_term_ouput_to_buf(buf, i - 1, ctx->shell->term_buffer,
                                 ctx->shell->cwd);

        print_term_buf(ctx->shell->term_inner_win, ctx->shell->term_buffer);

        memset(ctx->shell->buf, 0, BUFSIZ);

        cwd_len = strlen(ctx->shell->cwd);

        ctx->shell->term_buffer->current_col = cwd_len + (cwd_len > 0 ? 3 : 2);

        int nol = ctx->shell->term_buffer->num_of_lines;

        wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
              ctx->shell->term_buffer->current_col);

        ctx->shell->curr_buf_idx = 0;

        wnoutrefresh(ctx->shell->term_inner_win);
        doupdate();
    }
}

void append_term_ouput_to_buf(char *buf, int buf_len, TEXT_BUFFER *term_buf,
                              char *cwd)
{
    int cwd_len = strlen(cwd);

    int j = 0;
    int k = 0;
    int line_number = 0;

    LINE *curr_line = NULL;

    curr_line = term_buf->current_line;

    while (j < buf_len + 1)
    {
        if (j > 1 && buf[j] == '\0')
        {
            curr_line->buf_[k] = '\0';
            line_number++;
            curr_line->line_num = line_number;
            term_buf->num_of_lines++;
            term_buf->current_line->next = curr_line;
            curr_line->prev = term_buf->current_line;
            term_buf->current_line = curr_line;

            curr_line = initialize_line();
            line_number++;
            curr_line->line_num = line_number;
            term_buf->current_line->next = curr_line;
            curr_line->prev = term_buf->current_line;
            term_buf->current_line = curr_line;
            term_buf->current_line->next = NULL;
            break;
        }
        else if (buf[j] == '\r')
        {
            j++;
            continue;
        }
        else if (buf[j] == '\n')
        {
            curr_line->buf_[k] = '\0';
            k = 0;
            j++;
            line_number++;
            curr_line->line_num = line_number;
            term_buf->num_of_lines++;
            term_buf->current_line->next = curr_line;
            curr_line->prev = term_buf->current_line;
            term_buf->current_line = curr_line;
            curr_line = initialize_line();
            continue;
        }
        else if (k > (EDITOR_WIDTH - 20))
        {
            k = 0;
            line_number++;
            curr_line->line_num = line_number;
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

    line_number++;
    curr_line->line_num = line_number;
    term_buf->current_line->next = curr_line;
    curr_line->prev = term_buf->current_line;
    term_buf->current_line = curr_line;
    term_buf->current_line->next = NULL;

    char *new_prompt = malloc(cwd_len + 4);

    if (cwd_len > 0)
    {
        snprintf(new_prompt, cwd_len + 4, "%s > ", cwd);
        memcpy(term_buf->current_line->buf_, new_prompt, cwd_len + 4);
        term_buf->current_line->line_num = line_number;
    }
    else
    {
        memcpy(term_buf->current_line->buf_, "> ", 2);
        term_buf->current_line->line_num = line_number;
    }

    free(new_prompt);
}

void print_term_buf(WINDOW *term_win, TEXT_BUFFER *term_buf)
{
    werase(term_win);
    int nol = term_buf->num_of_lines;
    int start_line = nol < 8 ? 0 : nol - 8;

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

void *popen_w_pid_ret(void *arguments)
{
    THREAD_ARGS *args = (THREAD_ARGS *)arguments;

    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return NULL;
    }

    wrefresh(args->win);

    *args->pid = fork();
    if (*args->pid < 0)
    {
        perror("fork");
        return NULL;
    }

    if (*args->pid == 0)
    {
        setpgid(0, 0);
        // Child
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        execl("/bin/sh", "sh", "-c", args->cmd, (char *)NULL);
        perror("execl");
        _exit(1);
    }

    close(pipefd[1]);

    int status;
    waitpid(*args->pid, &status, WNOHANG);

    args->file = fdopen(pipefd[0], "r");
    if (!args->file)
    {
        perror("fdopen");
        close(pipefd[0]);
        return NULL;
    }

    fcntl(pipefd[0], F_SETFL, O_NONBLOCK);

    return (void *)args;
}

void run_output_funcs(APP_CONTEXT *ctx, char buf[])
{
    int cwd_len = strlen(ctx->shell->cwd);

    append_term_ouput_to_buf(buf, strlen(buf) - 1, ctx->shell->term_buffer,
                             ctx->shell->cwd);

    print_term_buf(ctx->shell->term_inner_win, ctx->shell->term_buffer);

    memset(ctx->shell->buf, 0, BUFSIZ);

    cwd_len = strlen(ctx->shell->cwd);
    ctx->shell->term_buffer->current_col = cwd_len + (cwd_len > 0 ? 3 : 2);

    int nol = ctx->shell->term_buffer->num_of_lines;

    wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
          ctx->shell->term_buffer->current_col);

    ctx->shell->curr_buf_idx = 0;

    wnoutrefresh(ctx->shell->term_inner_win);
    doupdate();
}

void *check_running_proc_for_output(void *arg)
{
    OUTPUT_THREAD_ARGS *args = (OUTPUT_THREAD_ARGS *)arg;
    char buf[BUFSIZ];

    int fd = fileno(args->file);
    fcntl(fd, F_SETFL, O_NONBLOCK);

    while (true)
    {
        ssize_t n = read(fd, buf, sizeof(buf) - 1);

        if (n > 0)
        {
            buf[n] = '\0';
            run_output_funcs(args->ctx, buf);
        }
        else if (n == 0)
        {
            run_output_funcs(args->ctx, "");
            args->ctx->shell->executable_running = false;
            break;
        }
        else
        {
            usleep(100);
        }
    }

    return NULL;
}
