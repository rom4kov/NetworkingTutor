#include "../../data/data_access_layer.h"
#include "../../views/views.h"
#include "../tests.h"
#include <CUnit/CUnit.h>
#include <stdio.h>
#include <string.h>

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

    int pattern_num = 4;

    pcre2_code *re[pattern_num];

    char *patterns[] = {
        "^\\s*struct\\s+addrinfo\\s+hints(\\s*=\\s*{[^}]*})?\\s*",
        "struct|",
        "case|default)\\b",
        "#(include|define)|NULL|=|\\+|\\-|\\*|\\&|<|>|;",
    };

    compile_patterns(re, pattern_num, patterns);

    for (int i = 0; i < pattern_num; i++)
    {
        while (buf_line)
        {
            rc = check_line_for_matches(buf_line, i, &re[i],
                                        strlen(buf_line->buf_));
            if (rc < 0)
                return -1;
            else
                buf_line = buf_line->next;
        }
    }

    fclose(server_c_file);

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
