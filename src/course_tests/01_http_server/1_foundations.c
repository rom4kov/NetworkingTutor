#include <CUnit/CUnit.h>
#include <stdio.h>

int answers_file_exists(char *path)
{
    FILE *answers_file = fopen(path, "r+");
    if (answers_file == NULL)
    {
        return 1;
    }

    return 0;
}

void test_if_answers_file_exists(void)
{
    CU_ASSERT(answers_file_exists("http_server/answers.txt"));
}
