#include "../include/disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <ncurses.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <mntent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define MAX_DISKS 32

typedef struct {
    char name[32];
    char mount_point[128];
    unsigned long total;
    unsigned long used;
    unsigned long free;
    int is_rotational;
    unsigned long reads;
    unsigned long writes;
    unsigned long read_sectors;
    unsigned long write_sectors;
} DiskInfo;

int get_mounts(DiskInfo *disks) {
    FILE *fp = setmntent("/proc/mounts", "r");
    if (!fp) return 0;

    struct mntent *ent;
    int count = 0;

    while ((ent = getmntent(fp)) && count < MAX_DISKS) {
        if (strncmp(ent->mnt_fsname, "/dev/", 5) == 0) {
            struct statvfs vfs;
            if (statvfs(ent->mnt_dir, &vfs) == 0) {
                strncpy(disks[count].name, ent->mnt_fsname + 5, sizeof(disks[count].name) - 1);
                strncpy(disks[count].mount_point, ent->mnt_dir, sizeof(disks[count].mount_point) - 1);
                disks[count].total = vfs.f_blocks * vfs.f_frsize;
                disks[count].free = vfs.f_bfree * vfs.f_frsize;
                disks[count].used = disks[count].total - disks[count].free;

                char rot_path[128];
                snprintf(rot_path, sizeof(rot_path), "/sys/block/%s/queue/rotational", disks[count].name);
                FILE *rot = fopen(rot_path, "r");
                if (rot) {
                    fscanf(rot, "%d", &disks[count].is_rotational);
                    fclose(rot);
                } else {
                    disks[count].is_rotational = -1;
                }

                // Read /proc/diskstats for read/write activity
                FILE *stats = fopen("/proc/diskstats", "r");
                if (stats) {
                    char line[512];
                    while (fgets(line, sizeof(line), stats)) {
                        char dev[32];
                        unsigned long r, rs, w, ws;
                        sscanf(line, "%*d %*d %31s %lu %*u %lu %*u %lu %*u %lu", dev, &r, &rs, &w, &ws);
                        if (strcmp(dev, disks[count].name) == 0) {
                            disks[count].reads = r;
                            disks[count].read_sectors = rs;
                            disks[count].writes = w;
                            disks[count].write_sectors = ws;
                            break;
                        }
                    }
                    fclose(stats);
                }
                count++;
            }
        }
    }
    endmntent(fp);
    return count;
}

void draw_disk_page() {
    timeout(0);
    int ch;
    int quit = 0;

    while (!quit) {
        DiskInfo disks[MAX_DISKS];
        int disk_count = get_mounts(disks);

        clear();
        attron(A_BOLD);
        mvprintw(0, 2, "[ DISK INFORMATION ]");
        attroff(A_BOLD);

        for (int i = 0; i < disk_count; i++) {
            float used_percent = (float)disks[i].used / (float)disks[i].total * 100.0f;
            mvprintw(2 + i * 5, 2, "%s (%s) - %.1f%% used (%s)",
                     disks[i].name,
                     disks[i].mount_point,
                     used_percent,
                     disks[i].is_rotational == 1 ? "HDD" : disks[i].is_rotational == 0 ? "SSD" : "Unknown");

            int bar_width = COLS - 20;
            int fill = (int)(bar_width * used_percent / 100.0);
            move(3 + i * 5, 4);
            if (used_percent > 90) attron(COLOR_PAIR(3));
            else if (used_percent > 60) attron(COLOR_PAIR(2));
            else attron(COLOR_PAIR(1));
            for (int j = 0; j < bar_width; j++) {
                addch(j < fill ? ACS_CKBOARD : ' ');
            }
            attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));

            mvprintw(4 + i * 5, 4, "Total: %.2f GB | Used: %.2f GB | Free: %.2f GB",
                     disks[i].total / (1024.0 * 1024 * 1024),
                     disks[i].used / (1024.0 * 1024 * 1024),
                     disks[i].free / (1024.0 * 1024 * 1024));

            mvprintw(5 + i * 5, 4, "Reads: %lu | Writes: %lu | ReadSectors: %lu | WriteSectors: %lu",
                     disks[i].reads, disks[i].writes, disks[i].read_sectors, disks[i].write_sectors);
        }

        mvprintw(LINES - 2, 2, "Press 'q' to return.");
        refresh();

        ch = getch();
        if (ch == 'q') quit = 1;
        usleep(1000000);  // refresh every 1s
    }
    timeout(-1);
}
