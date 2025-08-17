// #include <stdio.h>
// #include <time.h>
//
// int main(int argc, char *argv[])
// {
//     double time_diff_in_seconds = 0;
//     struct tm start_date;
//     struct tm end_date;
//     time_t start_time, end_time;
//
//     // char delim[] = "-";
//
//     // start_date.tm_year = atoi(strsep(&time1, delim));
//     // start_date.tm_mon = atoi(strsep(&time1, delim));
//     // start_date.tm_mday = atoi(strsep(&time1, delim));
//     // end_date.tm_year = atoi(strsep(&time2, delim));
//     // end_date.tm_mon = atoi(strsep(&time2, delim));
//     // end_date.tm_mday = atoi(strsep(&time2, delim));
//
//     start_date.tm_year = 2025;
//     start_date.tm_mon = 8;
//     start_date.tm_mday = 15;
//     start_date.tm_hour = 0;
//     start_date.tm_min = 0;
//     start_date.tm_sec = 0;
//
//     end_date.tm_year = 2025;
//     end_date.tm_mon = 8;
//     end_date.tm_mday = 16;
//     end_date.tm_hour = 0;
//     end_date.tm_min = 0;
//     end_date.tm_sec = 0;
//
//     printf("%i\n", start_date.tm_year);
//     printf("%i\n", start_date.tm_mon);
//     printf("%i\n", start_date.tm_mday);
//
//     printf("%i\n", end_date.tm_year);
//     printf("%i\n", end_date.tm_mon);
//     printf("%i\n", end_date.tm_mday);
//
//     start_time = mktime(&start_date); 
//     end_time = mktime(&end_date);
//     
//     printf("start_time: %ld\n", start_time);
//     printf("end_time: %ld\n", end_time);
//
//     time_diff_in_seconds = difftime(end_time, start_time);
//
//     printf("time diff in days: %.f\n", time_diff_in_seconds / (3600 * 24));
//
//     return time_diff_in_seconds;
// }
//
