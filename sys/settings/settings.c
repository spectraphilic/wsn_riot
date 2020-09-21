// Standard
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Riot
#include <vfs.h>

// Project
#include "settings.h"


struct Settings settings = {
    .log_level = LOG_DEBUG,
    .wan_type = WAN_DISABLED,
};


int settings_save(void) {
    int error;
    size_t size = 50;
    char buffer[size];

    int fd = vfs_open("/settings.txt", O_CREAT | O_WRONLY, 0);
    if (fd < 0) {
        return fd;
    }

    snprintf(buffer, size, "log.level = %d\n", settings.log_level);
    vfs_write(fd, buffer, strlen(buffer));

    snprintf(buffer, size, "wan.type = %d\n", settings.wan_type);
    vfs_write(fd, buffer, strlen(buffer));

    error = vfs_close(fd);
    return error;
}


int settings_load(void) {
    int error;

    int fd = vfs_open("/settings.txt", O_RDONLY, 0);
    if (fd < 0) {
        return fd;
    }

    // TODO

    error = vfs_close(fd);
    return error;
}
