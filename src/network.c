#include "../include/network.h"
#include <stdlib.h>
#include <ncurses.h>

int draw_network_page() {
    endwin();  // Exit ncurses to run external tool
    system("bmon");  // Launch bmon network monitor

    // Reinitialize ncurses for KernelSpy on return
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    return 1;  // Signal that this view is done immediately
}