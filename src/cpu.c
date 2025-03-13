#include "../include/cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

unsigned long long prev_total = 0, prev_idle = 0;

double get_cpu_usage() {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return -1;

    char buffer[1024];
    fgets(buffer, sizeof(buffer), fp);
    fclose(fp);

    unsigned long long user, nice, system, idle, iowait, irq, softirq;
    sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq);

    unsigned long long total = user + nice + system + idle + iowait + irq + softirq;
    unsigned long long total_idle = idle + iowait;

    if (prev_total == 0) {
        prev_total = total;
        prev_idle = total_idle;
        return 0;
    }

    double cpu_usage = 100.0 * (1 - (double)(total_idle - prev_idle) / (total - prev_total));
    prev_total = total;
    prev_idle = total_idle;

    return cpu_usage;
}

void draw_cpu_page() {
    double cpu = get_cpu_usage();
    clear();
    attron(A_BOLD);
    mvprintw(0, 2, "[ CPU USAGE ]");
    attroff(A_BOLD);

    if (cpu > 90) attron(COLOR_PAIR(3) | A_BLINK);
    else if (cpu > 50) attron(COLOR_PAIR(2));
    else attron(COLOR_PAIR(1));
    mvprintw(2, 4, "CPU Usage: %.2f%%", cpu);
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | A_BLINK);
    mvprintw(5, 2, "Press 'q' to return.");
    refresh();
}
