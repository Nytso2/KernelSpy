//
//  network.c
//  Kernel_Spy
//
//  Created by Luis on 3/12/25.
//

#include "../include/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ncurses.h>

double prev_rx = 0, prev_tx = 0;

const char *detect_network_interface() {
    static char iface[32];
    struct dirent *entry;
    DIR *dp = opendir("/sys/class/net/");
    if (!dp) return "eth0";

    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, "lo") != 0 && entry->d_name[0] != '.') {
            strncpy(iface, entry->d_name, sizeof(iface) - 1);
            closedir(dp);
            return iface;
        }
    }
    closedir(dp);
    return "eth0";
}

void get_network_usage(double *rx_rate, double *tx_rate) {
    char path[1024];
    const char *iface = detect_network_interface();
    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/rx_bytes", iface);

    FILE *fp = fopen(path, "r");
    double rx_bytes = 0, tx_bytes = 0;
    fscanf(fp, "%lf", &rx_bytes);
    fclose(fp);

    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/tx_bytes", iface);
    fp = fopen(path, "r");
    fscanf(fp, "%lf", &tx_bytes);
    fclose(fp);

    *rx_rate = (rx_bytes - prev_rx) / 1024.0;
    *tx_rate = (tx_bytes - prev_tx) / 1024.0;
    prev_rx = rx_bytes;
    prev_tx = tx_bytes;
}
void draw_network_page() {
    endwin();  // Close ncurses screen
    system("bmon");  // Launch external network monitor (you can change this to bmon, iftop, etc.)
    initscr();  // Restart ncurses
    cbreak();
    noecho();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
}
