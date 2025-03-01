#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include <sys/statvfs.h>
#include <dirent.h>


#define MAX_BUF 1024
#define UPDATE_INTERVAL 1 // in seconds

unsigned long long prev_total = 0;
unsigned long long prev_idle = 0;
double prev_rx = 0, prev_tx = 0;

// Automatically detect first non-loopback interface
const char *detect_network_interface() {
    static char iface[32];
    struct dirent *entry;
    DIR *dp = opendir("/sys/class/net/");
    if (dp == NULL) {
        return "eth0";  // Fallback if there's an error
    }

    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, "lo") != 0 && entry->d_name[0] != '.') {
            strncpy(iface, entry->d_name, sizeof(iface) - 1);
            iface[sizeof(iface) - 1] = '\0';
            closedir(dp);
            return iface;
        }
    }
    closedir(dp);
    return "eth0";  // Fallback if no valid interface is found
}

// Function to get CPU usage
double get_cpu_usage() {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return -1;

    char buffer[MAX_BUF];
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
        return 0;  // Skip first reading
    }

    double cpu_usage = 100.0 * (1 - (double)(total_idle - prev_idle) / (total - prev_total));

    prev_total = total;
    prev_idle = total_idle;

    return cpu_usage;
}

// Function to get memory usage excluding buffers/cache
double get_memory_usage() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return -1;

    double total, free, buffers, cached;
    char buffer[MAX_BUF];
    while (fgets(buffer, sizeof(buffer), fp)) {
        sscanf(buffer, "MemTotal: %lf", &total);
        sscanf(buffer, "MemFree: %lf", &free);
        sscanf(buffer, "Buffers: %lf", &buffers);
        sscanf(buffer, "Cached: %lf", &cached);
    }
    fclose(fp);

    double used_memory = total - free - buffers - cached;
    return 100.0 * (used_memory / total);
}

// Function to get disk usage for root `/`
double get_disk_usage() {
    struct statvfs stat;
    if (statvfs("/", &stat) != 0) return -1;
    double used = stat.f_blocks - stat.f_bfree;
    double total = stat.f_blocks;
    return 100.0 * (used / total);
}

// Function to get network usage speed in KB/s
void get_network_usage(double *rx_rate, double *tx_rate) {
    char path[MAX_BUF];
    const char *iface = detect_network_interface();

    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/rx_bytes", iface);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        *rx_rate = *tx_rate = -1;
        return;
    }

    double rx_bytes = 0, tx_bytes = 0;
    fscanf(fp, "%lf", &rx_bytes);
    fclose(fp);

    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/tx_bytes", iface);
    fp = fopen(path, "r");
    if (fp) {
        fscanf(fp, "%lf", &tx_bytes);
        fclose(fp);
    }

    if (prev_rx > 0 && prev_tx > 0) {
        *rx_rate = (rx_bytes - prev_rx) / 1024.0;
        *tx_rate = (tx_bytes - prev_tx) / 1024.0;
    } else {
        *rx_rate = *tx_rate = 0; // First pass
    }

    prev_rx = rx_bytes;
    prev_tx = tx_bytes;
}

// Function to get CPU temperature
double get_cpu_temperature() {
    FILE *fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (!fp) return -1;

    char buffer[MAX_BUF];
    fgets(buffer, sizeof(buffer), fp);
    fclose(fp);
    return atof(buffer) / 1000.0;
}

void terminal_bell() {
    beep();
}


void print_stats() {
    double cpu = get_cpu_usage();
    double memory = get_memory_usage();
    double disk = get_disk_usage();
    double temp = get_cpu_temperature();

    double rx_rate, tx_rate;
    get_network_usage(&rx_rate, &tx_rate);

    clear();
    attron(A_BOLD);
    mvprintw(0, 2, "SYSTEM MONITOR - LINUX DASHBOARD");
    attroff(A_BOLD);
    mvhline(1, 0, '=', COLS);

    // CPU
    attron(A_BOLD);
    mvprintw(3, 2, "[ CPU USAGE ]");
    attroff(A_BOLD);
    move(4, 4);
    if (cpu > 90) {
        attron(COLOR_PAIR(3) | A_BLINK);
        terminal_bell();
    } else if (cpu > 50) {
        attron(COLOR_PAIR(2));
    } else {
        attron(COLOR_PAIR(1));
    }
    printw("CPU Usage: %.2f%%", cpu);
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | A_BLINK);

    // Memory
    attron(A_BOLD);
    mvprintw(6, 2, "[ MEMORY USAGE ]");
    attroff(A_BOLD);
    move(7, 4);
    if (memory > 90) {
        attron(COLOR_PAIR(3) | A_BLINK);
        terminal_bell();
    } else if (memory > 50) {
        attron(COLOR_PAIR(2));
    } else {
        attron(COLOR_PAIR(1));
    }
    printw("Memory Usage: %.2f%%", memory);
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | A_BLINK);

    // Disk
    attron(A_BOLD);
    mvprintw(9, 2, "[ DISK USAGE ]");
    attroff(A_BOLD);
    move(10, 4);
    if (disk > 90) {
        attron(COLOR_PAIR(3) | A_BLINK);
        terminal_bell();
    } else if (disk > 50) {
        attron(COLOR_PAIR(2));
    } else {
        attron(COLOR_PAIR(1));
    }
    printw("Disk Usage: %.2f%%", disk);
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | A_BLINK);

    // Network (no colors here — it's not a danger metric)
    attron(A_BOLD);
    mvprintw(12, 2, "[ NETWORK USAGE ]");
    attroff(A_BOLD);
    mvprintw(13, 4, "Download: %.2f KB/s", rx_rate);
    mvprintw(14, 4, "Upload: %.2f KB/s", tx_rate);

    // Temperature
    attron(A_BOLD);
    mvprintw(16, 2, "[ CPU TEMPERATURE ]");
    attroff(A_BOLD);
    move(17, 4);
    if (temp > 75) {
        attron(COLOR_PAIR(3) | A_BLINK);
        terminal_bell();
    } else if (temp > 60) {
        attron(COLOR_PAIR(2));
    } else {
        attron(COLOR_PAIR(1));
    }
    printw("CPU Temp: %.2f°C", temp);
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | A_BLINK);

    mvhline(19, 0, '=', COLS);
    attron(A_DIM);
    mvprintw(20, 2, "Press Ctrl+C to exit");
    attroff(A_DIM);
    refresh();
}


int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    while (1) {
        print_stats();
        sleep(UPDATE_INTERVAL);
    }

    endwin();
    return 0;
//  to test ; use the follownig : stress-ng --vm 2 --vm-bytes 90% --vm-method all --timeout 5s

}
