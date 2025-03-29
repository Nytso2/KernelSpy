#include "../include/temp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <sensors/sensors.h>

void draw_temp_page() {
    sensors_init(NULL);
    const sensors_chip_name *chip;
    int chip_nr = 0;

    clear();
    attron(A_BOLD);
    mvprintw(0, 2, "[ TEMPERATURE INFORMATION ]");
    attroff(A_BOLD);

    int line = 2;
    int total_read = 0;
    double max_temp = -1000.0;
    double min_temp = 1000.0;
    double sum_temp = 0.0;

    while ((chip = sensors_get_detected_chips(NULL, &chip_nr)) != NULL) {
        const sensors_feature *feature;
        int feature_nr = 0;

        while ((feature = sensors_get_features(chip, &feature_nr)) != NULL) {
            if (feature->type == SENSORS_FEATURE_TEMP) {
                double value;
                if (sensors_get_value(chip, feature->number, &value) == 0) {
                    const char *label = sensors_get_label(chip, feature);
                    mvprintw(line++, 4, "%s: %.2f°C", label ? label : "Temp", value);
                    if (value > max_temp) max_temp = value;
                    if (value < min_temp) min_temp = value;
                    sum_temp += value;
                    total_read++;
                }
            }
        }
    }

    if (total_read > 0) {
        double avg_temp = sum_temp / total_read;
        mvprintw(line + 1, 4, "Average Temp: %.2f°C", avg_temp);
        mvprintw(line + 2, 4, "Max Temp:     %.2f°C", max_temp);
        mvprintw(line + 3, 4, "Min Temp:     %.2f°C", min_temp);
        line += 4;
    } else {
        mvprintw(line++, 4, "No temperature sensors found.");
    }

    mvprintw(line + 1, 2, "Press 'q' to return.");
    refresh();
    sensors_cleanup();
}
