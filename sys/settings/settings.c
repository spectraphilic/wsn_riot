// Standard
#include <errno.h>
#include <stdio.h>
#include <string.h>

// Posix
#include <fcntl.h>
#include <unistd.h>

// Riot
#include <fmt.h>
#include <log.h>

// Project
#include "settings.h"


struct Settings settings = {
    .wan_type = WAN_DISABLED,
};

const char * const settings_names[] = {
    "wan.type",
};

const size_t settings_len = sizeof settings_names / sizeof settings_names[0];


// TODO Move to a new module sys/compat
static int dprintf(int fd, const char *format, ...)
{
    int size = 255;
    char buffer[size];

    va_list args;
    va_start(args, format);
    int n = vsnprintf(buffer, size, format, args);
    va_end(args);

    if (n < 0) {
        return -1;
    }

    if (n > size - 1) { // XXX
        return -1;
    }

    return write(fd, buffer, n);
}


static char* dgets(int fd, char *str, int num)
{
    char c;
    int i;
    for (i = 0; i < num; i++) {
        ssize_t n = read(fd, &c, 1);
        if (n < 0) {
            return NULL;
        } else if (n == 0) {
            break;
        }

        str[i] = c;
        if (c == '\n') {
            i++;
            break;
        }
    }

    str[i] = '\0';
    return str;
}


int settings_index(const char *name)
{
    for (size_t i=0; i < settings_len; i++) {
        if (strcmp(name, settings_names[i]) == 0) {
            return i;
        }
    }

    LOG_WARNING("Unexpected settings name: %s\n", name);
    return -1;
}


int settings_set(const char *name, const char *value)
{
    int idx = settings_index(name);
    if (idx < 0) {
        return idx;
    }

    switch (idx) {
        uint32_t value_u32;
        case 0:
            value_u32 = scn_u32_dec(value, 1);
            if (value_u32 >= WAN_LEN) {
                LOG_WARNING("Unexpected value %s = %s\n", name, value);
                return -1;
            }
            settings.wan_type = (wan_type_t) value_u32;
            break;
    }

    return 0;
}


int settings_save(void)
{
    int fd = open("/settings.txt", O_WRONLY | O_CREAT);
    if (fd < 0) {
        LOG_ERROR("Failed to open settings.txt errno=%d\n", errno);
        return -1;
    }

    int error;
    dprintf(fd, "wan.type = %d\n", settings.wan_type);

    error = close(fd);
    if (error) {
        LOG_ERROR("Failed to close settings.txt\n");
    }
    return error;
}


int settings_load(void)
{
    int fd = open("/settings.txt", O_RDONLY);
    if (fd < 0) {
        LOG_ERROR("Failed to open settings.txt errno=%d\n", errno);
        return -1;
    }

    char buffer[255];
    char name[20];
    char value[20];
    while (dgets(fd, buffer, sizeof(buffer) - 1) != NULL) {
        if (buffer[0] == '\0') { // EOF
            break;
        }
        if (sscanf(buffer, "%s = %s", name, value) == 2) {
            settings_set(name, value);
        }
    }

    int error = close(fd);
    if (error) {
        LOG_ERROR("Failed to close settings.txt\n");
    }
    return error;
}
