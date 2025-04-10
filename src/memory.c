#include "../include/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

void get_memory_details(double *total, double *used, double *free_mem, double *buffers, double *cached, double *shared, double *available, double *swap_total, double *swap_free) {
    *total = *used = *free_mem = *buffers = *cached = *shared = *available = *swap_total = *swap_free = 0;

    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return;

    char label[64];
    double value;
    while (fscanf(fp, "%63[^:]: %lf kB\n", label, &value) == 2) {
        if (strcmp(label, "MemTotal") == 0) *total = value;
        else if (strcmp(label, "MemFree") == 0) *free_mem = value;
        else if (strcmp(label, "Buffers") == 0) *buffers = value;
        else if (strcmp(label, "Cached") == 0) *cached = value;
        else if (strcmp(label, "Shmem") == 0) *shared = value;
        else if (strcmp(label, "MemAvailable") == 0) *available = value;
        else if (strcmp(label, "SwapTotal") == 0) *swap_total = value;
        else if (strcmp(label, "SwapFree") == 0) *swap_free = value;
    }
    fclose(fp);

    if (*total > 0 && *available >= 0) {
        *used = *total - *available;
    } else {
        *used = 0;
    }
}

double get_memory_usage() {
    double total, used, free_mem, buffers, cached, shared, available, swap_total, swap_free;
    get_memory_details(&total, &used, &free_mem, &buffers, &cached, &shared, &available, &swap_total, &swap_free);
    return total > 0 ? 100.0 * (used / total) : 0;
}

void draw_memory_page() {
    int ch;
    nodelay(stdscr, TRUE);
    while ((ch = getch()) != 'q') {
        double total, used, free_mem, buffers, cached, shared, available, swap_total, swap_free;
        get_memory_details(&total, &used, &free_mem, &buffers, &cached, &shared, &available, &swap_total, &swap_free);
        double usage_percent = total > 0 ? 100.0 * (used / total) : 0;
        double swap_used = swap_total - swap_free;
        double swap_usage_percent = swap_total > 0 ? 100.0 * (swap_used / swap_total) : 0;

        clear();
        attron(A_BOLD);
        mvprintw(0, 2, "[ MEMORY INFORMATION ]");
        attroff(A_BOLD);

        mvprintw(2, 4, "Total RAM:      %.2f GB", total / 1024 / 1024);
        mvprintw(3, 4, "Used RAM:       %.2f GB", used / 1024 / 1024);
        mvprintw(4, 4, "Free RAM:       %.2f GB", free_mem / 1024 / 1024);
        mvprintw(5, 4, "Available RAM:  %.2f GB", available / 1024 / 1024);
        mvprintw(6, 4, "Buffers:        %.2f MB", buffers / 1024);
        mvprintw(7, 4, "Cached:         %.2f MB", cached / 1024);
        mvprintw(8, 4, "Shared:         %.2f MB", shared / 1024);
        mvprintw(9, 4, "Usage:          %.2f%%", usage_percent);

        int bar_width = COLS - 20;
        int fill = (int)(bar_width * usage_percent / 100.0);
        move(10, 4);
        if (usage_percent > 90) attron(COLOR_PAIR(3));
        else if (usage_percent > 50) attron(COLOR_PAIR(2));
        else attron(COLOR_PAIR(1));
        for (int i = 0; i < bar_width; i++) {
            addch(i < fill ? ACS_CKBOARD : ' ');
        }
        attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));

        mvprintw(12, 2, "Breakdown:");
        mvprintw(13, 4, "Used:      %.1f%%", total > 0 ? 100.0 * used / total : 0);
        mvprintw(14, 4, "Free:      %.1f%%", total > 0 ? 100.0 * free_mem / total : 0);
        mvprintw(15, 4, "Buffers:   %.1f%%", total > 0 ? 100.0 * buffers / total : 0);
        mvprintw(16, 4, "Cached:    %.1f%%", total > 0 ? 100.0 * cached / total : 0);
        mvprintw(17, 4, "Available: %.1f%%", total > 0 ? 100.0 * available / total : 0);

        mvprintw(19, 2, "Swap Total: %.2f GB", swap_total / 1024 / 1024);
        mvprintw(20, 2, "Swap Used:  %.2f GB", swap_used / 1024 / 1024);
        mvprintw(21, 2, "Swap Free:  %.2f GB", swap_free / 1024 / 1024);
        mvprintw(22, 2, "Swap Usage: %.2f%%", swap_usage_percent);

        mvprintw(LINES - 2, 2, "Press 'q' to return.");
        refresh();
        usleep(500000);  // update every 0.5s
    }
    nodelay(stdscr, FALSE);
}
