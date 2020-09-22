// Standard
#include <stdio.h>
#include <string.h>

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


int settings_index(const char *name)
{
    for (size_t i=0; i < settings_len; i++) {
        if (strcmp(name, settings_names[i]) == 0) {
            return i;
        }
    }

    LOG_ERROR("Unexpected settings name: %s", name);
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
                LOG_ERROR("Unexpected value %s = %s", name, value);
                return -1;
            }
            settings.wan_type = (wan_type_t) value_u32;
            break;
    }

    return 0;
}


int settings_save(void)
{
    FILE *fp = fopen("/settings.txt", "w");
    if (fp == NULL) {
        LOG_ERROR("Failed to open settings.txt");
        return -1;
    }

    int error;
    fprintf(fp, "wan.type = %d\n", settings.wan_type);

    error = fclose(fp);
    if (error) {
        LOG_ERROR("Failed to close settings.txt");
    }
    return error;
}


int settings_load(void)
{
    FILE *fp = fopen("/settings.txt", "r");
    if (fp == NULL) {
        LOG_ERROR("Failed to open settings.txt");
        return -1;
    }

    int error;
    char name[20];
    char value[20];
    while (fscanf(fp,"%s = %s", name, value) == 2) {
        settings_set(name, value);
    }

    error = fclose(fp);
    if (error) {
        LOG_ERROR("Failed to close settings.txt");
    }
    return error;
}
