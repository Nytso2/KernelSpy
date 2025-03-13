//
//  temp.c
//  Kernel_Spy
//
//  Created by Luis on 3/12/25.
//

#include "../include/temp.h"
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

double get_cpu_temperature() {
    FILE *fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (!fp) return -1;
    char buffer[1024];
    fgets(buffer, sizeof(buffer), fp);
    fclose(fp);
    return atof(buffer) / 1000.0;
}

void draw_temp_page() {
    double temp = get_cpu_temperature();
    clear();
    attron(A_BOLD);
    mvprintw(0, 2, "[ CPU TEMPERATURE ]");
    attroff(A_BOLD);

    if (temp > 75) attron(COLOR_PAIR(3) | A_BLINK);
    else if (temp > 60) attron(COLOR_PAIR(2));
    else attron(COLOR_PAIR(1));
    mvprintw(2, 4, "CPU Temperature: %.2f°C", temp);
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | A_BLINK);
    mvprintw(5, 2, "Press 'q' to return.");
    refresh();
}
