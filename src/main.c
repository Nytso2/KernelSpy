//
//  main.c
//  Kernel_Spy
//
//  Created by Luis on 3/12/25.
//

#include <ncurses.h>
#include "../include/cpu.h"
#include "../include/network.h"
#include "../include/memory.h"
#include "../include/disk.h"
#include "../include/temp.h"

const char *menu[] = { "CPU", "Network", "Memory", "Disk", "Temperature" };
#define NUM_MENU_ITEMS (sizeof(menu) / sizeof(menu[0]))

void draw_menu(int highlight) {
    clear();
    attron(A_BOLD);
    mvprintw(0, 2, "SYSTEM MONITOR - SELECT A CATEGORY");
    attroff(A_BOLD);
    mvhline(1, 0, '=', COLS);

    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
        if (i == highlight) attron(A_REVERSE);
        mvprintw(3 + i, 4, "%s", menu[i]);
        if (i == highlight) attroff(A_REVERSE);
    }

    mvhline(10, 0, '=', COLS);
    attron(A_DIM);
    mvprintw(12, 2, "Use Arrow Keys to navigate, Enter to select, q to quit.");
    attroff(A_DIM);
    refresh();
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    int highlight = 0;
    int c;

    while (1) {
        draw_menu(highlight);
        c = getch();

        if (c == 'q') break;
        else if (c == KEY_UP) highlight = (highlight - 1 + NUM_MENU_ITEMS) % NUM_MENU_ITEMS;
        else if (c == KEY_DOWN) highlight = (highlight + 1) % NUM_MENU_ITEMS;
        else if (c == 10) { // Enter
            while (1) {
                if (highlight == 0) draw_cpu_page();
                else if (highlight == 1) draw_network_page();
                else if (highlight == 2) draw_memory_page();
                else if (highlight == 3) draw_disk_page();
                else if (highlight == 4) draw_temp_page();

                c = getch();
                if (c == 'q') break;
            }
        }
    }

    endwin();
    return 0;
}

