#include "../include/cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#define MAX_CORES 128

unsigned long long prev_total[MAX_CORES];
unsigned long long prev_idle[MAX_CORES];

int get_core_count() {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return 1;
    int cores = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "cpu", 3) == 0 && isdigit(line[3])) {
            cores++;
        }
    }
    fclose(fp);
    return cores;
}

double get_cpu_usage_core(int core) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return -1;

    char buffer[1024];
    for (int i = 0; i <= core; i++) {
        fgets(buffer, sizeof(buffer), fp);
    }
    fclose(fp);

    unsigned long long user, nice, system, idle, iowait, irq, softirq;
    sscanf(buffer, "cpu%*d %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq);

    unsigned long long total = user + nice + system + idle + iowait + irq + softirq;
    unsigned long long total_idle = idle + iowait;

    double usage = 0.0;
    if (prev_total[core] > 0) {
        usage = 100.0 * (1.0 - (double)(total_idle - prev_idle[core]) / (total - prev_total[core]));
    }

    prev_total[core] = total;
    prev_idle[core] = total_idle;

    return usage;
}

double get_cpu_usage() {
    return get_cpu_usage_core(0);
}

void get_load_average(double *load1, double *load5, double *load15) {
    FILE *fp = fopen("/proc/loadavg", "r");
    if (!fp) return;
    fscanf(fp, "%lf %lf %lf", load1, load5, load15);
    fclose(fp);
}

double get_cpu_frequency() {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) return -1;
    char line[256];
    double freq = -1;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "cpu MHz\t: %lf", &freq) == 1) {
            break;
        }
    }
    fclose(fp);
    return freq / 1000.0; // Convert MHz to GHz
}

unsigned long long get_context_switches() {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return 0;
    char line[256];
    unsigned long long ctxt = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "ctxt %llu", &ctxt) == 1) {
            break;
        }
    }
    fclose(fp);
    return ctxt;
}

unsigned long long get_interrupts() {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return 0;
    char line[256];
    unsigned long long intr = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "intr %llu", &intr) == 1) {
            break;
        }
    }
    fclose(fp);
    return intr;
}

void draw_cpu_page() {
    int cores = get_core_count();
    double load1, load5, load15;
    get_load_average(&load1, &load5, &load15);
    double freq = get_cpu_frequency();
    unsigned long long ctxt = get_context_switches();
    unsigned long long intr = get_interrupts();

    clear();
    attron(A_BOLD);
    mvprintw(0, 2, "[ CPU INFORMATION ]");
    attroff(A_BOLD);

    mvprintw(2, 4, "Cores: %d", cores);
    mvprintw(3, 4, "Frequency: %.2f GHz", freq);
    mvprintw(4, 4, "Load Avg (1/5/15m): %.2f %.2f %.2f", load1, load5, load15);
    mvprintw(5, 4, "Context Switches: %llu", ctxt);
    mvprintw(6, 4, "Interrupts: %llu", intr);

    for (int i = 0; i < cores; i++) {
        double usage = get_cpu_usage_core(i + 1); // skip 'cpu' line
        mvprintw(8 + i, 4, "Core %2d: %.2f%%", i, usage);

        int bar_width = COLS - 20;
        int fill = (int)(bar_width * usage / 100.0);
        move(8 + i, 20);
        if (usage > 90) attron(COLOR_PAIR(3));
        else if (usage > 50) attron(COLOR_PAIR(2));
        else attron(COLOR_PAIR(1));
        for (int j = 0; j < bar_width; j++) {
            addch(j < fill ? ACS_CKBOARD : ' ');
        }
        attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));
    }

    mvprintw(9 + cores, 2, "Press 'q' to return.");
    refresh();
}
