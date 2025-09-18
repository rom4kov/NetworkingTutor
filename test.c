#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int main_stat(void)
{
    char *home_dir = getenv("HOME");
    char docs_dir_buf[64];
    char ntutor_dir[64];
    char *proj_dir_buf = calloc(64, 1);
    struct stat s;

    snprintf(docs_dir_buf, strlen(home_dir) + 11, "%s/Documents", home_dir);

    int err = stat(docs_dir_buf, &s);

    if (-1 == err)
    {
        if (ENOENT == errno)
        {
            snprintf(ntutor_dir, strlen(home_dir) + 8, "%s/ntutor", home_dir);
            snprintf(proj_dir_buf,
                     strlen(home_dir) + 18 + strlen("Romaonv"),
                     "%s/ntutor/%s", home_dir, "Romanov");
        }
        else
        {
            perror("stat");
        }
    }
    else
    {
        if (S_ISDIR(s.st_mode))
        {
            snprintf(ntutor_dir, strlen(home_dir) + 18, "%s/Documents/ntutor",
                     home_dir);
            snprintf(proj_dir_buf,
                     strlen(home_dir) + 19 + strlen("Romanov"),
                     "%s/Documents/ntutor/%s", home_dir, "Romanov");
        }
    }
    printf("%i\n", err);
    printf("%s\n", home_dir);
    printf("%s\n", docs_dir_buf);
    printf("%i\n", S_ISDIR(s.st_mode));
    printf("%s\n", ntutor_dir);
    printf("%s\n", proj_dir_buf);
    return 0;
}


