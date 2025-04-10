#include "../include/cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_CORES 128

typedef struct {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
} CpuStats;

CpuStats prev_stats[MAX_CORES];
int num_cores = 0;

void read_cpu_stats(CpuStats *stats) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return;

    char line[512];
    int idx = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "cpu", 3) != 0 || !isdigit(line[3])) continue;
        sscanf(line, "cpu%d %llu %llu %llu %llu %llu %llu %llu %llu",
               &idx, &stats[idx].user, &stats[idx].nice, &stats[idx].system, &stats[idx].idle,
               &stats[idx].iowait, &stats[idx].irq, &stats[idx].softirq, &stats[idx].steal);
        idx++;
    }
    fclose(fp);
    num_cores = idx;
}

float compute_usage(CpuStats prev, CpuStats curr) {
    unsigned long long prev_idle = prev.idle + prev.iowait;
    unsigned long long curr_idle = curr.idle + curr.iowait;

    unsigned long long prev_non_idle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    unsigned long long curr_non_idle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;

    unsigned long long prev_total = prev_idle + prev_non_idle;
    unsigned long long curr_total = curr_idle + curr_non_idle;

    float totald = (float)(curr_total - prev_total);
    float idled = (float)(curr_idle - prev_idle);

    return (totald - idled) / totald * 100.0f;
}

float get_core_freq(int core) {
    char path[128];
    snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", core);
    FILE *fp = fopen(path, "r");
    if (!fp) return 0;
    int kHz;
    fscanf(fp, "%d", &kHz);
    fclose(fp);
    return kHz / 1000.0f;
}

void get_cpu_model(char *model, size_t size) {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) return;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0) {
            char *colon = strchr(line, ':');
            if (colon) {
                strncpy(model, colon + 2, size - 1);
                model[strcspn(model, "\n")] = '\0';
                break;
            }
        }
    }
    fclose(fp);
}

void get_load_avg(double *load1, double *load5, double *load15) {
    FILE *fp = fopen("/proc/loadavg", "r");
    if (!fp) return;
    fscanf(fp, "%lf %lf %lf", load1, load5, load15);
    fclose(fp);
}

int get_cpu_cores() {
    int count = 0;
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) return 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "processor", 9) == 0) count++;
    }
    fclose(fp);
    return count;
}

void draw_cpu_page() {
    timeout(0);
    int ch;
    int quit = 0;

    CpuStats curr_stats[MAX_CORES];
    read_cpu_stats(prev_stats);
    usleep(500000);

    char model[128] = "Unknown";
    get_cpu_model(model, sizeof(model));
    int core_count = get_cpu_cores();

    while (!quit) {
        read_cpu_stats(curr_stats);
        double load1 = 0, load5 = 0, load15 = 0;
        get_load_avg(&load1, &load5, &load15);

        clear();
        attron(A_BOLD);
        mvprintw(0, 2, "[ CPU INFORMATION ]");
        attroff(A_BOLD);

        mvprintw(1, 2, "Model: %s", model);
        mvprintw(2, 2, "Cores: %d", core_count);
        mvprintw(3, 2, "Load Average (1m, 5m, 15m): %.2f %.2f %.2f", load1, load5, load15);

        for (int i = 0; i < num_cores; i++) {
            float usage = compute_usage(prev_stats[i], curr_stats[i]);
            float freq = get_core_freq(i);

            mvprintw(5 + i * 2, 2, "Core %d: Usage: %5.1f%%  Freq: %.0f MHz", i, usage, freq);

            int bar_width = COLS - 40;
            int fill = (int)(bar_width * usage / 100.0);
            move(6 + i * 2, 4);
            if (usage > 90) attron(COLOR_PAIR(3));
            else if (usage > 50) attron(COLOR_PAIR(2));
            else attron(COLOR_PAIR(1));
            for (int j = 0; j < bar_width; j++) {
                addch(j < fill ? ACS_CKBOARD : ' ');
            }
            attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));
        }

        mvprintw(LINES - 4, 2, "Architecture: %s", sizeof(void*) == 8 ? "x86_64 (64-bit)" : "x86 (32-bit)");
        mvprintw(LINES - 3, 2, "Clock source: %s", access("/sys/devices/system/clocksource/clocksource0/current_clocksource", F_OK) == 0 ? "system" : "unknown");
        mvprintw(LINES - 2, 2, "Press 'q' to return.");
        refresh();
        memcpy(prev_stats, curr_stats, sizeof(CpuStats) * MAX_CORES);
        ch = getch();
        if (ch == 'q') quit = 1;
        usleep(500000);
    }
    timeout(-1);
}