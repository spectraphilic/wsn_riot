// Standard Library
#include <stdio.h>

// Riot
#include <log.h>

// Project
#include "settings.h"


const char * const help[] = {
    "0=disabled 1=4g 2=iridium", // wan.type
};


static int get(int8_t idx) {
    switch (idx) {
        case 0:
            printf("%d\n", settings.wan_type);
            return 0;
        default:
            return -1;
    }
}


extern int cmd_var(int argc, char **argv) {
    int error = 0;

    if (argc == 1) {
        // Help
        for (size_t i=0; i < settings_len; i++) {
            printf("%s %s\n", settings_names[i], help[i]);
        }
    } else if (argc == 2) {
        // Get
        int idx = settings_index(argv[1]);
        error = get(idx);
    } else if (argc == 3) {
        // Set
        error = settings_set(argv[1], argv[2]);
        if (error) {
            return error;
        }
        error = settings_save();
        if (error) {
            return error;
        }
    } else {
        LOG_ERROR("Unexpected number of arguments: %d\n", argc);
        return -1;
    }

    return error;
}
