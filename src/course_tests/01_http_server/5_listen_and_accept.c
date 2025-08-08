#define _POSIX_C_SOURCE 2
#include "../../data/data_access_layer.h"
#include "../../views/views.h"
#include "../tests.h"
#include "http_server_tests.h"
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

int server_c_contains_listen_syscall(char *path)
{
    int rc;

    FILE *server_c_file = fopen(path, "r+");
    if (server_c_file == NULL)
    {
        return 1;
    }

    TEXT_BUFFER *text_buf = initialize_buffer();

    read_file_into_buffer(server_c_file, text_buf);
    LINE *buf_line = text_buf->first_line;

    int pattern_num = 1;

    pcre2_code *re[pattern_num];

    char *patterns[] = {"\\blisten\\s*\\(\\s*[^,]+,\\s*[^,]+,\\s*[^)]+\\)"};

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < pattern_num; i++)
    {
        bool pattern_matched = false;
        while (buf_line)
        {
            rc =
                check_line_for_matches(buf_line, i, re, strlen(buf_line->buf_));
            if (rc >= 0)
                pattern_matched = true;

            buf_line = buf_line->next;
        }
        if (!pattern_matched)
        {
            fclose(server_c_file);
            return 1;
        }
        buf_line = text_buf->first_line;
    }

    fclose(server_c_file);

    return 0;
}

int server_c_contains_accept_syscall(char *path)
{
    int rc;

    FILE *server_c_file = fopen(path, "r+");
    if (server_c_file == NULL)
    {
        return 1;
    }

    TEXT_BUFFER *text_buf = initialize_buffer();

    read_file_into_buffer(server_c_file, text_buf);
    LINE *buf_line = text_buf->first_line;

    int pattern_num = 1;

    pcre2_code *re[pattern_num];

    char *patterns[] = {"\\baccept\\s*\\(\\s*[^,]+,\\s*[^,]+,\\s*[^)]+\\)"};

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < pattern_num; i++)
    {
        bool pattern_matched = false;
        while (buf_line)
        {
            rc =
                check_line_for_matches(buf_line, i, re, strlen(buf_line->buf_));
            if (rc >= 0)
                pattern_matched = true;

            buf_line = buf_line->next;
        }
        if (!pattern_matched)
        {
            fclose(server_c_file);
            return 1;
        }
        buf_line = text_buf->first_line;
    }

    fclose(server_c_file);

    return 0;
}

void test_if_server_c_contains_listen_syscall(void)
{
    bool is_socket_call_is_present = false;
    if (server_c_contains_listen_syscall("http_server/server.c") == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void test_if_server_c_contains_accept_syscall(void)
{
    bool is_socket_call_is_present = false;
    if (server_c_contains_accept_syscall("http_server/server.c") == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void register_section5_tests(APP_CONTEXT *ctx)
{
    ctx->sp[4] = CU_add_suite("http_server_04", NULL, NULL);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[4], "server.c file exists",
                (CU_TestFunc)test_if_server_c_file_exists);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[4], "server.c file compiles without errors",
                (CU_TestFunc)test_if_server_c_file_compiles_without_errors);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[4], "server.c contains listen syscall",
                (CU_TestFunc)test_if_server_c_contains_listen_syscall);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[4], "server.c contains listen syscall",
                (CU_TestFunc)test_if_server_c_contains_accept_syscall);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }
    //
    // CU_add_test(ctx->sp[4],
    //             "server.c contains fprintf or perror for functions in for loop",
    //             (CU_TestFunc)test_if_server_c_contains_fprintf_or_perror);
    // ctx->ec = CU_get_error();
    // if (ctx->ec != CUE_SUCCESS)
    // {
    //     const char *err_msg = CU_get_error_msg();
    //     mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    // }
    //
    // CU_add_test(
    //     ctx->sp[4], "socket(), setsockopt() and bind() are inside a for loop",
    //     (CU_TestFunc)test_if_server_c_has_working_for_loop_for_picking_addr);
    // ctx->ec = CU_get_error();
    // if (ctx->ec != CUE_SUCCESS)
    // {
    //     const char *err_msg = CU_get_error_msg();
    //     mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    // }
    //
    // CU_add_test(ctx->sp[4], "socket syscall in server.c file works",
    //             (CU_TestFunc)test_if_socket_syscall_worked_in_server_c);
    // ctx->ec = CU_get_error();
    // if (ctx->ec != CUE_SUCCESS)
    // {
    //     const char *err_msg = CU_get_error_msg();
    //     mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    // }
    //
    // CU_add_test(ctx->sp[4], "bind syscall in server.c file works",
    //             (CU_TestFunc)test_if_bind_syscall_in_server_c_works);
    // ctx->ec = CU_get_error();
    // if (ctx->ec != CUE_SUCCESS)
    // {
    //     const char *err_msg = CU_get_error_msg();
    //     mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    // }
}
