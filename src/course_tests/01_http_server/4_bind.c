#include "../../../ntutor.h"
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
#include <pcre2.h>

int server_c_contains_bind_syscall(char *path)
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

    char *patterns[] = {"\\bbind\\s*\\(\\s*[^,]+,\\s*[^,]+,\\s*[^)]+\\)"};

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

int server_c_contains_setsockopt_call(char *path)
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

    int pattern_num = 2;

    pcre2_code *re[pattern_num];

    char *patterns[] = {"\\bsetsockopt\\s*\\(\\s*[^,]+,\\s*[^,]+,\\s*[^)]+\\)"};

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

int server_c_contains_fprintf_or_perror(char *path, int num)
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

    int pattern_num = 2;

    pcre2_code *re[pattern_num];

    char *patterns[] = {"\\bfprintf\\s*\\(\\s*[^,]+,\\s*[^,]+,\\s*[^)]+\\)",
                        "\\bperror\\s*\\(\\s*[^,]+\\s*\\)"};

    compile_patterns(re, pattern_num, patterns);

    int pattern_matched = 0;
    for (int i = 0; i < pattern_num; i++)
    {
        while (buf_line)
        {
            rc =
                check_line_for_matches(buf_line, i, re, strlen(buf_line->buf_));
            if (rc >= 0)
                pattern_matched++;

            buf_line = buf_line->next;
        }
        buf_line = text_buf->first_line;
    }

    fclose(server_c_file);

    if (pattern_matched == num)
        return 0;
    else
        return 1;
}

int server_c_has_working_for_loop_for_picking_addr()
{
    FILE *fp = popen(
        "perl -0777 -ne 'print \"inside\\n\" if "
        "/for\\s*\\(.*?\\)\\s*\\{.*?socket\\(.*?\\).*?"
        "setsockopt\\(.*?\\).*?bind\\(.*?\\)/s' http_server/server.c",
        "r");

    if (!fp)
    {
        perror("popen");
        return 1;
    }

    char buffer[256];
    size_t len = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[len] = '\0';

    pclose(fp);

    return strstr(buffer, "inside") ? 0 : 1;
}

int bind_syscall_works()
{
    FILE *fp = popen("perl -nE 'say $1 if /\\b(\\w+)\\s*=\\s*bind\\s*\\(/' "
                     "http_server/server.c",
                     "r");
    if (fp == NULL)
    {
        perror("popen failed");
        return 1;
    }

    char buffer[32];
    char c;
    int i = 0;
    while (fread(&c, 1, 1, fp))
    {
        buffer[i] = c;
        i++;
    }
    buffer[i] = '\0';
    if (i < 2)
        return 1;

    char perl_command[512];

    snprintf(perl_command, sizeof(perl_command),
             "perl -0777 -pe "
             "'s/(freeaddrinfo\\s*\\(\\s*res\\s*\\)\\s*;)/"
             "printf(\\\"%%i\\\\n\\\", %s);\\n    \\1/' "
             "http_server/server.c > http_server/server_modified2.c",
             buffer);

    FILE *fp2 = popen(perl_command, "r");
    if (fp2 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    FILE *fp3 =
        popen("gcc http_server/server_modified2.c -o "
              "http_server/server_modified2 && ./http_server/server_modified2",
              "r");
    if (fp3 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    remove("http_server/server_modified2.c");
    remove("http_server/server_modified2");

    int rc;
    fread(&rc, 1, 1, fp3);
    if (!(rc >= 0))
    {
        return 1;
    }

    return 0;
}

void test_if_server_c_contains_bind_syscall(void)
{
    bool is_socket_call_is_present = false;
    if (server_c_contains_bind_syscall("http_server/server.c") == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void test_if_server_c_contains_fprintf_or_perror()
{
    bool is_socket_call_is_present = false;
    if (server_c_contains_fprintf_or_perror("http_server/server.c", 4) == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void test_if_server_c_has_working_for_loop_for_picking_addr(void)
{
    bool is_socket_call_is_present = false;
    if (server_c_has_working_for_loop_for_picking_addr() == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void test_if_bind_syscall_in_server_c_works(void)
{
    bool is_socket_call_is_present = false;
    if (bind_syscall_works() == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void register_section4_tests(APP_CONTEXT *ctx)
{
    ctx->sp[3] = CU_add_suite("http_server_04", NULL, NULL);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[3], "server.c file exists",
                (CU_TestFunc)test_if_server_c_file_exists);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[3], "server.c file compiles without errors",
                (CU_TestFunc)test_if_server_c_file_compiles_without_errors);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[3], "server.c contains bind syscall",
                (CU_TestFunc)test_if_server_c_contains_bind_syscall);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[3],
                "server.c contains fprintf or perror for functions in for loop",
                (CU_TestFunc)test_if_server_c_contains_fprintf_or_perror);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(
        ctx->sp[3], "socket(), setsockopt() and bind() are inside a for loop",
        (CU_TestFunc)test_if_server_c_has_working_for_loop_for_picking_addr);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[3], "socket syscall in server.c file works",
                (CU_TestFunc)test_if_socket_syscall_worked_in_server_c);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[3], "bind syscall in server.c file works",
                (CU_TestFunc)test_if_bind_syscall_in_server_c_works);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }
}
