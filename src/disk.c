//
//  disk.c
//  Kernel_Spy
//
//  Created by Luis on 3/12/25.
//

#include "../include/disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <ncurses.h>

double get_disk_usage() {
    struct statvfs stat;
    if (statvfs("/", &stat) != 0) return -1;
    double used = stat.f_blocks - stat.f_bfree;
    double total = stat.f_blocks;
    return 100.0 * (used / total);
}

void draw_disk_page() {
    double disk = get_disk_usage();
    clear();
    attron(A_BOLD);
    mvprintw(0, 2, "[ DISK USAGE ]");
    attroff(A_BOLD);

    if (disk > 90) attron(COLOR_PAIR(3) | A_BLINK);
    else if (disk > 50) attron(COLOR_PAIR(2));
    else attron(COLOR_PAIR(1));
    mvprintw(2, 4, "Disk Usage: %.2f%%", disk);
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | A_BLINK);
    mvprintw(5, 2, "Press 'q' to return.");
    refresh();
}
