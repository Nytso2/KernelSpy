#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>
#include "../include/cpu.h"
#include "../include/network.h"
#include "../include/memory.h"
#include "../include/disk.h"

const char *menu[] = { "[ CPU ]", "[ Network ]", "[ Memory ]", "[ Disk ]" };
#define NUM_MENU_ITEMS (sizeof(menu) / sizeof(menu[0]))

void draw_footer() {
    char timebuf[64];
    time_t now = time(NULL);
    strftime(timebuf, sizeof(timebuf), "%a %b %d %H:%M:%S", localtime(&now));

    attron(COLOR_PAIR(3));
    mvhline(LINES - 3, 1, ACS_HLINE, COLS - 2);
    attroff(COLOR_PAIR(3));
    attron(A_DIM);
    mvprintw(LINES - 2, 2, "Use Arrow Keys to navigate | Enter to select | q to quit | Time: %s", timebuf);
    attroff(A_DIM);
}

void draw_menu(size_t highlight) {
    clear();
    box(stdscr, 0, 0);

    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(1, (COLS - 27) / 2, "[ KernelSpy System Monitor ]");
    attroff(A_BOLD | COLOR_PAIR(2));

    mvhline(2, 1, ACS_HLINE, COLS - 2);

    for (size_t i = 0; i < NUM_MENU_ITEMS; i++) {
        if (i == highlight) {
            attron(COLOR_PAIR(1) | A_REVERSE);
            mvprintw(4 + i * 2, (COLS - strlen(menu[i])) / 2, "%s", menu[i]);
            attroff(COLOR_PAIR(1) | A_REVERSE);
        } else {
            mvprintw(4 + i * 2, (COLS - strlen(menu[i])) / 2, "%s", menu[i]);
        }
    }

    draw_footer();
    refresh();
}

int draw_network_page();

int main() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();

    use_default_colors();
    init_pair(1, COLOR_GREEN, -1);   // Menu highlight
    init_pair(2, COLOR_BLACK, -1);    // Header
    init_pair(3, COLOR_YELLOW, -1);  // Footer line

    size_t highlight = 0;
    int c;

    while (1) {
        draw_menu(highlight);
        c = getch();

        if (c == 'q') break;
        else if (c == KEY_UP) highlight = (highlight - 1 + NUM_MENU_ITEMS) % NUM_MENU_ITEMS;
        else if (c == KEY_DOWN) highlight = (highlight + 1) % NUM_MENU_ITEMS;
        else if (c == 10) { // Enter key
            int exit_view = 0;
            while (1) {
                if (highlight == 0) draw_cpu_page();
                else if (highlight == 1) exit_view = draw_network_page();
                else if (highlight == 2) draw_memory_page();
                else if (highlight == 3) draw_disk_page();

                if (exit_view) break;

                c = getch();
                if (c == 'q') break;
            }
        }
    }

    endwin();
    return 0;
}
