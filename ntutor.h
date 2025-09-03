#define _DEFAULT_SOURCE 1
#define _POSIX_C_SOURCE 200809L
#define COLOR_GREY 16
#define COLOR_ORANGE 17
#define COLOR_DARKGREY 18
#define COLOR_TEALGREEN 19
#define COLOR_BERMUDA 20

#define GREETER_PANEL_COUNT 3
#define START_WINDOW_COUNT 6

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4
#define EDITOR_WIDTH ((WU * 7 + 4) - (WU + WU / 2))

#define WINDOW_COUNT 4
#define WU COLS / 12 // WU for WIDTH_UNIT
#define WIDTH_REMAINDER COLS % WU
#define EXPLORER_WIDTH WU + WU / 2
#define EDIT_WIN_WIDTH WU * 5 + WU / 2
#define SHELL_WINDOW_IDX 4

#define MAX_COURSE_SECTIONS 16
