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

    char *patterns[] = {"\\blisten\\s*\\(\\s*[^,]+,\\s*[^,]+\\)"};

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

int listen_syscall_works()
{
    FILE *fp = popen("perl -nE 'say $1 if /\\b(\\w+)\\s*=\\s*listen\\s*\\(/' "
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

    int rc;
    fread(&rc, 1, 1, fp3);
    if (!(rc >= 0))
    {
        return 1;
    }

    remove("http_server/server_modified2.c");
    remove("http_server/server_modified2");

    return 0;
}

int accept_syscall_works()
{
    FILE *fp = popen("perl -nE 'say $1 if /\\b(\\w+)\\s*=\\s*accept\\s*\\(/' "
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
        popen("gcc http_server/server_modified3.c -o "
              "http_server/server_modified3 && ./http_server/server_modified3",
              "r");
    if (fp3 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    int rc;
    fread(&rc, 1, 1, fp3);
    if (!(rc >= 0))
    {
        return 1;
    }

    remove("http_server/server_modified2.c");
    remove("http_server/server_modified2");

    return 0;
}

int syscall_error_handling_works(char *syscall)
{
    char perl_command[256];

    snprintf(perl_command, sizeof(perl_command),
             "perl -nE 'say $1 if /\\b(\\w+)\\s*=\\s*%s\\s*\\(/' "
             "http_server/server.c",
             syscall);

    FILE *fp = popen(perl_command, "r");
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

    pclose(fp);

    char perl_command2[512];

    snprintf(perl_command2, sizeof(perl_command2),
             "perl -0777 -pe 's/(\\n.*\\s*=\\s*%s)/    %s = "
             "111;\\1/' http_server/server.c > http_server/server_modified3.c",
             syscall, buffer);

    system(perl_command2);

    FILE *fp2 = popen(
        "/usr/bin/gcc http_server/server_modified3.c -o "
        "http_server/server_modified3 && ./http_server/server_modified3 2>&1",
        "r");
    if (fp2 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    char buffer2[128];
    i = 0;
    while (fread(&c, 1, 1, fp2))
    {
        buffer2[i] = c;
        i++;
    }
    buffer2[i] = '\0';

    if (strstr(buffer2, "Address family not supported by protocol") == NULL)
    {
        pclose(fp2);
        return 1;
    }
    pclose(fp2);

    remove("http_server/server_modified3.c");
    remove("http_server/server_modified3");

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

void test_if_listen_syscall_in_server_c_works(void)
{
    bool is_socket_call_is_present = false;
    if (listen_syscall_works() == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void test_if_accept_syscall_in_server_c_works(void)
{
    bool is_socket_call_is_present = false;
    if (accept_syscall_works() == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void test_if_server_c_contains_fprintf_or_perror2(void)
{
    bool is_socket_call_is_present = false;
    if (server_c_contains_fprintf_or_perror("http_server/server.c", 7) == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void test_if_listen_error_handling_works(void)
{
    bool does_syscall_error_handling_work = false;
    if (syscall_error_handling_works("listen") == 0)
    {
        does_syscall_error_handling_work = true;
    }
    CU_ASSERT(does_syscall_error_handling_work);
}

void test_if_accept_error_handling_works(void)
{
    bool does_syscall_error_handling_work = false;
    if (syscall_error_handling_works("accept") == 0)
    {
        does_syscall_error_handling_work = true;
    }
    CU_ASSERT(does_syscall_error_handling_work);
}

void register_section5_tests(APP_CONTEXT *ctx)
{
    ctx->sp[4] = CU_add_suite("http_server_05", NULL, NULL);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(ctx->sp[4], "server.c file exists",
                (CU_TestFunc)test_if_server_c_file_exists);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(ctx->sp[4], "server.c file compiles without errors",
                (CU_TestFunc)test_if_server_c_file_compiles_without_errors);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(ctx->sp[4], "server.c contains listen syscall",
                (CU_TestFunc)test_if_server_c_contains_listen_syscall);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(ctx->sp[4], "server.c contains accept syscall",
                (CU_TestFunc)test_if_server_c_contains_accept_syscall);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }

    CU_add_test(ctx->sp[4],
                "server.c contains fprintf or perror where necessary",
                (CU_TestFunc)test_if_server_c_contains_fprintf_or_perror2);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    }
    //
    // CU_add_test(ctx->sp[4], "listen syscall in server.c file works",
    //             (CU_TestFunc)test_if_listen_syscall_in_server_c_works);
    // ctx->ec = CU_get_error();
    // if (ctx->ec != CUE_SUCCESS)
    // {
    //     const char *err_msg = CU_get_error_msg();
    //     mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    // }
    //
    // CU_add_test(ctx->sp[4], "accept syscall in server.c file works",
    //             (CU_TestFunc)test_if_accept_syscall_in_server_c_works);
    // ctx->ec = CU_get_error();
    // if (ctx->ec != CUE_SUCCESS)
    // {
    //     const char *err_msg = CU_get_error_msg();
    //     mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    // }
    //
    // CU_add_test(ctx->sp[4], "listen error handling in server.c file works",
    //             (CU_TestFunc)test_if_listen_error_handling_works);
    // ctx->ec = CU_get_error();
    // if (ctx->ec != CUE_SUCCESS)
    // {
    //     const char *err_msg = CU_get_error_msg();
    //     mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    // }
    //
    // CU_add_test(ctx->sp[4], "accept error handling in server.c file works",
    //             (CU_TestFunc)test_if_accept_error_handling_works);
    // ctx->ec = CU_get_error();
    // if (ctx->ec != CUE_SUCCESS)
    // {
    //     const char *err_msg = CU_get_error_msg();
    //     mvwprintw(ctx->course_windows[4], 1, 1, "%s", err_msg);
    // }
}
