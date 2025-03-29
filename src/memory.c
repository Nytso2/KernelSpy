//
//  memory.c
//  Kernel_Spy
//
//  Created by Luis on 3/12/25.
//

#include "../include/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

void get_memory_stats(double *total, double *used, double *free_mem, double *buffers, double *cached) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return;

    char label[64];
    double value;
    while (fscanf(fp, "%63s %lf", label, &value) == 2) {
        if (strcmp(label, "MemTotal:") == 0) *total = value;
        else if (strcmp(label, "MemFree:") == 0) *free_mem = value;
        else if (strcmp(label, "Buffers:") == 0) *buffers = value;
        else if (strcmp(label, "Cached:") == 0) *cached = value;
    }
    fclose(fp);

    *used = *total - *free_mem - *buffers - *cached;
}

double get_memory_usage() {
    double total, used, free_mem, buffers, cached;
    get_memory_stats(&total, &used, &free_mem, &buffers, &cached);
    return 100.0 * (used / total);
}

void draw_memory_page() {
    double total, used, free_mem, buffers, cached;
    get_memory_stats(&total, &used, &free_mem, &buffers, &cached);
    double usage_percent = 100.0 * (used / total);

    clear();
    attron(A_BOLD);
    mvprintw(0, 2, "[ MEMORY USAGE ]");
    attroff(A_BOLD);

    mvprintw(2, 4, "Total:   %.2f GB", total / 1024 / 1024);
    mvprintw(3, 4, "Used:    %.2f GB", used / 1024 / 1024);
    mvprintw(4, 4, "Free:    %.2f GB", free_mem / 1024 / 1024);
    mvprintw(5, 4, "Buffers: %.2f MB", buffers / 1024);
    mvprintw(6, 4, "Cached:  %.2f MB", cached / 1024);

    mvprintw(8, 4, "Usage:   %.2f%%", usage_percent);

    int bar_width = COLS - 20;
    int fill = (int)(bar_width * usage_percent / 100.0);
    move(9, 4);
    if (usage_percent > 90) attron(COLOR_PAIR(3));
    else if (usage_percent > 50) attron(COLOR_PAIR(2));
    else attron(COLOR_PAIR(1));
    for (int i = 0; i < bar_width; i++) {
        addch(i < fill ? ACS_CKBOARD : ' ');
    }
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));

    mvprintw(11, 2, "Press 'q' to return.");
    refresh();
}
