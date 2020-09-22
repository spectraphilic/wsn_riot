// Standard
#include <stdio.h>
#include <string.h>

// Riot
#include <fmt.h>

// Project
#include "settings.h"


struct Settings settings = {
    .log_level = LOG_DEBUG,
    .wan_type = WAN_DISABLED,
};

const char * const settings_names[] = {
    "log.level",
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

    return -1;
}


int settings_set(const char *name, const char *value) {
    int idx = settings_index(name);
    switch (idx) {
        case 0:
            settings.log_level = (log_level_t) scn_u32_dec(value, 1);
            return 0;
        case 1:
            settings.wan_type = (wan_type_t) scn_u32_dec(value, 1);
            return 0;
    }

    return -1;
}


int settings_save(void) {
    FILE *fp = fopen("/settings.txt", "w");
    if (fp == NULL) {
        return -1;
    }

    int error;
    fprintf(fp, "log.level = %d\n", settings.log_level);
    fprintf(fp, "wan.type = %d\n", settings.wan_type);

    error = fclose(fp);
    return error;
}


int settings_load(void) {
    FILE *fp = fopen("/settings.txt", "r");
    if (fp == NULL) {
        return -1;
    }

    int error;
    char name[20];
    char value[20];
    while (fscanf(fp,"%s = %s", name, value) == 2) {
        settings_set(name, value);
    }

    error = fclose(fp);
    return error;
}
