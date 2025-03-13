//
//  memory.c
//  Kernel_Spy
//
//  Created by Luis on 3/12/25.
//

#include "../include/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

double get_memory_usage() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return -1;

    double total, free, buffers, cached;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {
        sscanf(buffer, "MemTotal: %lf", &total);
        sscanf(buffer, "MemFree: %lf", &free);
        sscanf(buffer, "Buffers: %lf", &buffers);
        sscanf(buffer, "Cached: %lf", &cached);
    }
    fclose(fp);
    return 100.0 * ((total - free - buffers - cached) / total);
}

void draw_memory_page() {
    double memory = get_memory_usage();
    clear();
    attron(A_BOLD);
    mvprintw(0, 2, "[ MEMORY USAGE ]");
    attroff(A_BOLD);

    if (memory > 90) attron(COLOR_PAIR(3) | A_BLINK);
    else if (memory > 50) attron(COLOR_PAIR(2));
    else attron(COLOR_PAIR(1));
    mvprintw(2, 4, "Memory Usage: %.2f%%", memory);
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | A_BLINK);
    mvprintw(5, 2, "Press 'q' to return.");
    refresh();
}
