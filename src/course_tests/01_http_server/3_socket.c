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

int socket_call_is_present(char *path)
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

    char *patterns[] = {"\\bsocket\\s*\\(\\s*[^,]+,\\s*[^,]+,\\s*[^)]+\\)",
                        "\\bfprintf\\s*\\(\\s*[^,]+,\\s*[^,]+,\\s*[^)]+\\)"};

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

int socket_syscall_works()
{
    FILE *fp = popen("perl -nE 'say $1 if /\\b(\\w+)\\s*=\\s*socket\\s*\\(/' "
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
    buffer[i - 1] = '\0';

    char perl_command[512];

    snprintf(perl_command, sizeof(perl_command),
             "perl -0777 -pe "
             "'s/(freeaddrinfo\\s*\\(\\s*res\\s*\\)\\s*;)/"
             "printf(\\\"%%i\\\\n\\\", %s);\\n    \\1/' "
             "http_server/server.c > http_server/server_modified.c",
             buffer);

    FILE *fp2 = popen(perl_command, "r");
    if (fp2 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    FILE *fp3 =
        popen("gcc http_server/server_modified.c -o "
              "http_server/server_modified && ./http_server/server_modified",
              "r");
    if (fp3 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    int sockfd;
    fread(&sockfd, 1, 1, fp3);
    if (sockfd == -1)
    {
        return 1;
    }

    return 0;
}

int socket_syscall_error_handling_works(void)
{
    system("perl -0777 -pe 's/(\\n.*\\s*=\\s*socket)/    res->ai_family = "
           "111;\\1/' http_server/server.c > http_server/server_modified.c");

    FILE *fp2 = popen(
        "/usr/bin/gcc http_server/server_modified.c -o "
        "http_server/server_modified && ./http_server/server_modified 2>&1",
        "r");
    if (fp2 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    char buffer[128];
    char c;
    int i = 0;
    while (fread(&c, 1, 1, fp2))
    {
        buffer[i] = c;
        i++;
    }
    buffer[i] = '\0';

    if (strstr(buffer, "Address family not supported by protocol") == NULL)
    {
        pclose(fp2);
        return 1;
    }
    pclose(fp2);

    return 0;
}

void test_if_server_c_contains_socket_syscall(void)
{
    bool is_socket_call_is_present = false;
    if (socket_call_is_present("http_server/server.c") == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void test_if_socket_syscall_worked_in_server_c(void)
{
    bool did_socket_syscall_work = false;
    if (socket_syscall_error_handling_works() == 0)
    {
        did_socket_syscall_work = true;
    }
    CU_ASSERT(did_socket_syscall_work);
}

void test_if_socket_syscall_error_handling_works(void)
{
    bool is_socket_call_is_present = false;
    if (socket_syscall_error_handling_works() == 0)
    {
        is_socket_call_is_present = true;
    }
    CU_ASSERT(is_socket_call_is_present);
}

void register_section3_tests(APP_CONTEXT *ctx)
{
    ctx->sp[2] = CU_add_suite("http_server_03", NULL, NULL);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[2], "server.c file exists",
                (CU_TestFunc)test_if_server_c_file_exists);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[2],
                "server.c contains socket syscall and fprintf function",
                (CU_TestFunc)test_if_server_c_contains_socket_syscall);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[2], "server.c file compiles without errors",
                (CU_TestFunc)test_if_server_c_file_compiles_without_errors);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[2],
                "socket syscall error handling in server.c file works",
                (CU_TestFunc)test_if_socket_syscall_error_handling_works);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[2], "socket syscall in server.c file works",
                (CU_TestFunc)test_if_socket_syscall_worked_in_server_c);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }
}
