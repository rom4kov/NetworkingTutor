#define _POSIX_C_SOURCE 2
#include "../../data/data_access_layer.h"
#include "../../views/views.h"
#include "../tests.h"
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

int server_c_file_exists(char *path)
{
    FILE *server_file = fopen(path, "r+");
    if (server_file == NULL)
    {
        return 1;
    }
    fclose(server_file);

    return 0;
}

int server_c_file_contains_correct_hints_struct_decl(char *path)
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

    int pattern_num = 3;

    pcre2_code *re[pattern_num];

    char *patterns[] = {
        "^\\s*struct\\s+addrinfo\\s+hints(\\s*=\\s*{[^}]*})?\\s*",
        "^\\s*hints\\.ai_family\\s+=\\s+AF_UNSPEC;",
        "^\\s*hints\\.ai_socktype\\s+=\\s+SOCK_STREAM;",
        // "hints.ai_socktype",
        // "case|default)\\b",
        // "#(include|define)|NULL|=|\\+|\\-|\\*|\\&|<|>|;",
    };

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

int server_c_file_contains_getaddrinfo_call(char *path)
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

    char *patterns[] = {
        "\\s?getaddrinfo\\s?\\(",
    };

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

int server_c_file_compiles_without_errors()
{
    FILE *fp;

    FILE *output_file = fopen("http_server/output.txt", "w+");
    if (output_file == NULL)
    {
        return 1;
    }

    char output[1035];

    fp = popen("/usr/bin/gcc http_server/server.c -o server", "r");
    if (fp == NULL)
    {
        fprintf(stderr, "%s", "server.c failed to compile");
        return 0;
    }

    while (fgets(output, sizeof(output), fp))
    {
        fwrite(output, 1, 1, output_file);
    }

    fclose(output_file);
    pclose(fp);

    return 0;
}

int server_c_file_handles_getaddrinfo_errors()
{
    FILE *fp =
        popen("perl -0777 -pe "
              "'s/(getaddrinfo\\([^,]+,\\s*)[^,\\n]+/\\1\\\"error\\\"/"
              "g' http_server/server.c > http_server/server_error_test.c",
              "r");
    if (fp == NULL)
    {
        perror("popen failed");
        return 1;
    }

    pclose(fp);

    FILE *fp2 =
        popen("gcc http_server/server_error_test.c -o http_server/server_error "
              "&& ./http_server/server_error 2>&1",
              "r");
    if (fp2 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    char buffer[2000];
    char c;
    int i = 0;
    while (fread(&c, 1, 1, fp2))
    {
        buffer[i] = c;
        i++;
    }
    buffer[i] = '\0';

    if (strstr(buffer, "Servname not supported for ai_socktype") == NULL)
    {
        pclose(fp2);
        return 1;
    }

    remove("http_server/server_error_test.c");
    remove("http_server/server_error");

    pclose(fp2);
    return 0;
}

int server_c_file_returns_correct_results_from_getaddrinfo()
{
    FILE *fp = popen("perl -0777 -pe "
                     "'s/(freeaddrinfo\\s*\\(\\s*res\\s*\\)\\s*;)/"
                     "printf(\\\"%i\\\\n\\\", res->ai_family);\\n    \\1/' "
                     "http_server/server.c > http_server/server_modified.c",
                     "r");
    if (fp == NULL)
    {
        perror("popen failed");
        return 1;
    }

    pclose(fp);

    FILE *fp2 = popen(
        "gcc http_server/server_modified.c -o http_server/server_modified "
        "&& ./http_server/server_modified",
        "r");
    if (fp2 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    char buffer[2000];
    char c;
    int i = 0;
    while (fread(&c, 1, 1, fp2))
    {
        buffer[i] = c;
        i++;
    }
    buffer[i] = '\0';

    if (strstr(buffer, "2") == NULL)
    {
        pclose(fp2);
        return 1;
    }

    remove("http_server/server_modified");

    pclose(fp2);
    return 0;
}

int server_c_file_uses_freeaddrinfo_for_results()
{
    FILE *fp = popen("gcc http_server/server.c -o http_server/server "
                     "&& ./http_server/server",
                     "r");
    if (fp == NULL)
    {
        perror("popen failed");
        return 1;
    }

    FILE *fp2 = popen("valgrind ./http_server/server 2>&1", "r");
    if (fp2 == NULL)
    {
        perror("popen failed");
        return 1;
    }

    char buffer[2000];
    char c;
    int i = 0;
    while (fread(&c, 1, 1, fp2))
    {
        buffer[i] = c;
        i++;
    }
    buffer[i] = '\0';

    pclose(fp2);

    if (strstr(buffer, "All heap blocks were freed") == NULL)
    {
        return 1;
    }

    return 0;
}

void test_if_server_c_file_exists(void)
{
    bool does_server_file_exist = false;
    if (server_c_file_exists("http_server/server.c") == 0)
    {
        does_server_file_exist = true;
    }
    CU_ASSERT(does_server_file_exist);
}

void test_if_server_c_file_contains_hints_struct_decl(void)
{
    bool does_server_file_contain_correct_hints_struct_decl = false;
    if (server_c_file_contains_correct_hints_struct_decl(
            "http_server/server.c") == 0)
    {
        does_server_file_contain_correct_hints_struct_decl = true;
    }
    CU_ASSERT(does_server_file_contain_correct_hints_struct_decl);
}

void test_if_server_c_file_contains_getaddrinfo_call(void)
{
    bool does_server_file_contain_getaddrinfo_call = false;
    if (server_c_file_contains_getaddrinfo_call("http_server/server.c") == 0)
    {
        does_server_file_contain_getaddrinfo_call = true;
    }
    CU_ASSERT(does_server_file_contain_getaddrinfo_call);
}

void test_if_server_c_file_compiles_without_errors(void)
{
    bool does_server_file_compile_without_errors = false;
    if (server_c_file_compiles_without_errors() == 0)
    {
        does_server_file_compile_without_errors = true;
    }
    CU_ASSERT(does_server_file_compile_without_errors);
}

void test_if_server_c_file_handles_getaddrinfo_errors(void)
{
    bool does_server_file_handle_getaddrinfo_errors = false;
    if (server_c_file_handles_getaddrinfo_errors() == 0)
    {
        does_server_file_handle_getaddrinfo_errors = true;
    }
    CU_ASSERT(does_server_file_handle_getaddrinfo_errors);
}

void test_if_server_c_file_returns_correct_results_from_getaddrinfo(void)
{
    bool does_server_return_correct_results_from_getaddrinfo = false;
    if (server_c_file_returns_correct_results_from_getaddrinfo() == 0)
    {
        does_server_return_correct_results_from_getaddrinfo = true;
    }
    CU_ASSERT(does_server_return_correct_results_from_getaddrinfo);
}

void test_if_server_c_file_uses_freeaddrinfo_for_results(void)
{
    bool does_server_c_file_uses_freeaddrinfo_for_results = false;
    if (server_c_file_uses_freeaddrinfo_for_results() == 0)
    {
        does_server_c_file_uses_freeaddrinfo_for_results = true;
    }
    CU_ASSERT(does_server_c_file_uses_freeaddrinfo_for_results);
}

void register_section2_tests(APP_CONTEXT *ctx)
{
    ctx->sp[1] = CU_add_suite("http_server_02", NULL, NULL);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[1], "server.c file exists",
                (CU_TestFunc)test_if_server_c_file_exists);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[1], "server.c file contains hints struct declaration",
                (CU_TestFunc)test_if_server_c_file_contains_hints_struct_decl);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[1], "server.c file contains getaddrinfo call",
                (CU_TestFunc)test_if_server_c_file_contains_getaddrinfo_call);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[1], "server.c file compiles without errors",
                (CU_TestFunc)test_if_server_c_file_compiles_without_errors);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(ctx->sp[1], "server.c file handles getaddrinfo errors",
                (CU_TestFunc)test_if_server_c_file_handles_getaddrinfo_errors);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(
        ctx->sp[1], "server.c file returns correct results from getaddrinfo",
        (CU_TestFunc)
            test_if_server_c_file_returns_correct_results_from_getaddrinfo);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }

    CU_add_test(
        ctx->sp[1], "server.c file uses freeaddrinfo to free results struct",
        (CU_TestFunc)test_if_server_c_file_uses_freeaddrinfo_for_results);
    ctx->ec = CU_get_error();
    if (ctx->ec != CUE_SUCCESS)
    {
        const char *err_msg = CU_get_error_msg();
        mvwprintw(ctx->course_windows[4], 1, 0, "%s", err_msg);
    }
}
