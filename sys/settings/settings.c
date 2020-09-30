// Standard
#include <errno.h>

// Riot
#include <fmt.h>
#include <log.h>

// Project
#include <triage.h>
#include "settings.h"


struct Settings settings = {
    .wan_type = WAN_DISABLED,
};

const char * const settings_names[] = {
    "wan.type",
};

const size_t settings_len = sizeof settings_names / sizeof settings_names[0];


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

const char *settings_name(size_t index)
{
    if (index >= settings_len) {
        return NULL;
    }
    return settings_names[index];
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
    const char *filename = "/settings.txt";

    int fd = open(filename, O_WRONLY | O_CREAT);
    if (fd < 0) {
        char err[16];
        errno_string(errno, err, sizeof(err));
        LOG_ERROR("Failed to open %s (%s)\n", filename, err);
        return -1;
    }

    int error;
    dprintf(fd, "wan.type = %d\n", settings.wan_type);

    error = close(fd);
    if (error) {
        LOG_ERROR("Failed to close %s\n", filename);
    }
    return error;
}


int settings_load(void)
{
    const char *filename = "/settings.txt";

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        char err[16];
        errno_string(errno, err, sizeof(err));
        LOG_ERROR("Failed to open %s (%s)\n", filename, err);
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
        LOG_ERROR("Failed to close %s\n", filename);
    }
    return error;
}
