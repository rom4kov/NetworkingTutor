#include "../../ntutor.h"
#include "../controllers/controllers.h"
#include "../core/core.h"
#include "../models/models.h"

#include <curses.h>
#include <errno.h>
#include <fcntl.h>
#include <ncurses.h>
#include <pthread.h>
#include <pty.h>
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
        int cwd_len = strlen(ctx->shell->cwd);

        for (int i = 0; i < strlen(ctx->shell->buf); i++)
        {
            mvwaddch(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
                     i + (cwd_len > 0 ? cwd_len + 3 : 2), ' ');
        }
        // mvwprintw(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7, 2, "%s",
        //           "                   ");
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

    int cwd_len = strlen(ctx->shell->cwd);
    mvwprintw(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
              2 + (cwd_len > 0 ? cwd_len + 1 : 0), "%s", ctx->shell->buf);

    wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
          ctx->shell->term_buffer->current_col);

    ctx->shell->curr_buf_idx += 1;

    wnoutrefresh(ctx->edit_window);
    wnoutrefresh(ctx->shell->term_inner_win);
    doupdate();
}

bool cmd_is_cd(APP_CONTEXT *ctx)
{
    const char delim[] = " ";
    char *shell_buf = strdup(ctx->shell->buf);

    const char *first_cmd_part = strsep(&shell_buf, delim);

    if (strcmp(ctx->shell->buf, "cd") == 0)
    {
        ctx->shell->cwd = strdup("");
        int rv = (chdir(ctx->shell->home_dir));
        mvwprintw(ctx->edit_window, 37, 4, "home dir: %s",
                  ctx->shell->home_dir);
        if (rv == -1)
            mvwprintw(ctx->edit_window, 38, 4, "chdir error: %s",
                      strerror(errno));
        else if (rv == 0)
            mvwprintw(ctx->edit_window, 38, 4, "chdir success");
        return false;
    }
    else if (strcmp(first_cmd_part, "cd") == 0)
    {
        ctx->shell->cwd = strdup(strsep(&shell_buf, delim));
        return true;
    }

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
        malloc(len + 1 + 5 + 2); // original + null + " 2>&1" + '\n' + '\0'
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
        // mvwprintw(ctx->edit_window, 1, 30, "Before running program");

        THREAD_ARGS args = {.cmd = command,
                            .file = out,
                            .pid = malloc(sizeof(pid_t)),
                            .win = ctx->edit_window};
        pthread_t run_executable;

        pthread_create(&run_executable, NULL, popen_w_pid_ret, &args);

        pthread_join(run_executable, (void **)&args);
        napms(200);

        OUTPUT_THREAD_ARGS output_thr_args = {0};
        output_thr_args.file = args.file;
        output_thr_args.pid = malloc(sizeof(pid_t));
        output_thr_args.ctx = ctx;
        output_thr_args.win = ctx->edit_window;
        output_thr_args.stop_flag = 0;

        pthread_t check_for_output;
        pthread_create(&check_for_output, NULL, check_running_proc_for_output,
                       &output_thr_args);
        // output_thr_args.stop_flag = 1;
        pthread_join(check_for_output, NULL);
        // mvwprintw(ctx->edit_window, 4, 30, "Child pid: %i\n", *args.pid);
        // wrefresh(ctx->edit_window);
    }
    else
    {
        FILE *fp = popen(command, "r");
        while (fread(&c, 1, 1, fp) == 1)
        {
            if (i < BUFSIZ - 1)
            {
                buf[i++] = c;
            }
            else
            {
                // buffer full, maybe flush or break
                break;
            }
        }
        if (i > 0)
            buf[i - 1] = '\0';
        else
            buf[i] = '\0';

        pclose(fp);

        // mvwprintw(ctx->edit_window, 23, 5, "output: %s", command);
        // wrefresh(ctx->edit_window);

        ctx->shell->buf[ctx->shell->curr_buf_idx] = '\0';

        if (cmd_is_cd(ctx))
        {
            chdir(ctx->shell->cwd);
            // mvwprintw(ctx->edit_window, 28, 5, "get_cwd: %s", get_cwd());
            // mvwprintw(ctx->edit_window, 29, 5, "home_dir: %s",
            //           ctx->shell->home_dir);
            if (strcmp(get_cwd(), ctx->shell->home_dir) == 0)
            {
                ctx->shell->cwd = strdup("");
            }
            else
            {
                ctx->shell->cwd = get_cwd_base_name();
                ctx->shell->cwd[strlen(ctx->shell->cwd) - 1] = '\0';
            }
        }

        ctx->shell->buf[ctx->shell->curr_buf_idx] = '\n';
        ctx->shell->buf[ctx->shell->curr_buf_idx + 1] = '\0';

        memcpy(ctx->shell->term_buffer->current_line->prev->buf_ + cwd_len +
                   (cwd_len > 0 ? 3 : 2),
               ctx->shell->buf, strlen(ctx->shell->buf));

        append_term_ouput_to_buf(ctx->edit_window, buf, i - 1,
                                 ctx->shell->term_buffer, ctx->shell->cwd);

        print_term_buf(ctx->shell->term_inner_win, ctx->shell->term_buffer);

        // mvwprintw(ctx->edit_window, 23, 5, "buf: %s", ctx->shell->buf);

        memset(ctx->shell->buf, 0, BUFSIZ);

        cwd_len = strlen(ctx->shell->cwd);
        // mvwprintw(ctx->edit_window, 15, 5, "cwd_len: %i", cwd_len);
        ctx->shell->term_buffer->current_col = cwd_len + (cwd_len > 0 ? 3 : 2);

        int nol = ctx->shell->term_buffer->num_of_lines;
        // mvwprintw(ctx->edit_window, 20, 5, "nol: %i", nol);
        // mvwprintw(ctx->edit_window, 21, 5, "num_of_lines: %i",
        //           ctx->shell->term_buffer->num_of_lines);

        wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
              ctx->shell->term_buffer->current_col);

        ctx->shell->curr_buf_idx = 0;

        wnoutrefresh(ctx->edit_window);
        wnoutrefresh(ctx->shell->term_inner_win);
        doupdate();
    }
}

void append_term_ouput_to_buf(WINDOW *win, char *buf, int buf_len,
                              TEXT_BUFFER *term_buf, char *cwd)
{
    int cwd_len = strlen(cwd);
    // mvwprintw(win, 15, 50, "cwd_len in append: %i", cwd_len);
    int j = 0;
    int k = 0;
    int line_number = 0;
    // mvwprintw(win, 1, 3, "buf_len :: %i", buf_len);
    // mvwprintw(win, 2, 3, "cwd :: %s", cwd);

    LINE *curr_line = initialize_line();

    curr_line = term_buf->current_line;
    // mvwprintw(win, 4, 3, "curr_line->buf_ :: %s", curr_line->buf_);

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
    // mvwprintw(win, 9, 3, "%s", term_buf->current_line->buf_);
}

void print_term_buf(WINDOW *term_win, TEXT_BUFFER *term_buf)
{
    werase(term_win);
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

void *popen_w_pid_ret(void *arguments)
{
    THREAD_ARGS *args = (THREAD_ARGS *)arguments;
    // mvwprintw(args->win, 9, 30, "%s", "test popen_w_pid_ret");
    // wrefresh(args->win);
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return NULL;
    }
    // mvwprintw(args->win, 10, 30, "%s", "test popen_w_pid_ret after pipe");
    wrefresh(args->win);

    *args->pid = fork();
    if (*args->pid < 0)
    {
        perror("fork");
        return NULL;
    }
    // mvwprintw(args->win, 11, 30, "%s", "test popen_w_pid_ret after fork");
    // wrefresh(args->win);

    if (*args->pid == 0)
    {
        // Child
        close(pipefd[0]);               // close read end
        dup2(pipefd[1], STDOUT_FILENO); // redirect stdout to pipe
        dup2(pipefd[1], STDERR_FILENO); // redirect stderr too (optional)
        close(pipefd[1]);

        execl("/bin/sh", "sh", "-c", args->cmd, (char *)NULL);
        perror("execl"); // only reached if exec fails
        _exit(1);
    }
    // mvwprintw(args->win, 12, 30, "%s", "test popen_w_pid_ret after execl");
    // wrefresh(args->win);

    // Parent
    close(pipefd[1]); // close write end

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

    return (void *)args; // parent gets child PID directly
}

void run_output_funcs(APP_CONTEXT *ctx, char buf[BUFSIZ])
{
    int cwd_len = strlen(ctx->shell->cwd);

    // ctx->shell->buf[ctx->shell->curr_buf_idx] = '\n';
    // ctx->shell->buf[ctx->shell->curr_buf_idx + 1] = '\0';

    // memcpy(ctx->shell->term_buffer->current_line->prev->buf_ + cwd_len +
    //            (cwd_len > 0 ? 3 : 2),
    //        ctx->shell->buf, strlen(ctx->shell->buf));
    //
    // mvwprintw(ctx->edit_window, 13, 5, "cwd: %s end", ctx->shell->cwd);
    append_term_ouput_to_buf(ctx->edit_window, buf, strlen(buf) - 1,
                             ctx->shell->term_buffer, ctx->shell->cwd);
    // mvwprintw(ctx->edit_window, 14, 5, "cwd: %s end", ctx->shell->cwd);

    print_term_buf(ctx->shell->term_inner_win, ctx->shell->term_buffer);

    // mvwprintw(ctx->edit_window, 23, 5, "buf: %s", ctx->shell->buf);

    memset(ctx->shell->buf, 0, BUFSIZ);

    cwd_len = strlen(ctx->shell->cwd);
    // mvwprintw(ctx->edit_window, 15, 5, "%s", "cwd_len:    hello");
    // mvwprintw(ctx->edit_window, 15, 5, "cwd_len: %i", cwd_len);
    ctx->shell->term_buffer->current_col = cwd_len + (cwd_len > 0 ? 3 : 2);

    int nol = ctx->shell->term_buffer->num_of_lines;
    // mvwprintw(ctx->edit_window, 20, 5, "nol: %i", nol);
    // mvwprintw(ctx->edit_window, 21, 5, "num_of_lines: %i",
    //           ctx->shell->term_buffer->num_of_lines);

    wmove(ctx->shell->term_inner_win, nol < 8 ? nol - 1 : 7,
          ctx->shell->term_buffer->current_col);

    ctx->shell->curr_buf_idx = 0;

    wnoutrefresh(ctx->edit_window);
    wnoutrefresh(ctx->shell->term_inner_win);
    doupdate();
}

void *check_running_proc_for_output(void *arg)
{
    OUTPUT_THREAD_ARGS *args = (OUTPUT_THREAD_ARGS *)arg;
    char buf[BUFSIZ];
    // mvwprintw(args->ctx->course_windows[2], 5, 50, "%s", "in checking func");
    // wrefresh(args->ctx->course_windows[2]);

    int fd = fileno(args->file);
    fcntl(fd, F_SETFL, O_NONBLOCK); // make reads non-blocking

    while (!args->stop_flag)
    {
        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0)
        {
            buf[n] = '\0';
            // call your function to append/print to ncurses window
            run_output_funcs(args->ctx, buf);
        }
        else if (n == 0)
        {
            // EOF reached; child closed output
            break;
        }
        else
        {
            // no data right now
            usleep(50000); // 50ms sleep to avoid busy-waiting
        }
    }

    return NULL;
}
