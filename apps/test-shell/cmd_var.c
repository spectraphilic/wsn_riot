// Standard Library
#include <stdio.h>

// Riot
#include <fmt.h>

// Project
#include "settings.h"


const char * const help[] = {
    "0=off 1=fatal 2=error 3=warn 4=info 5=debug 6=trace", // log.level
    "0=disabled 1=4g 2=iridium", // wan.type
};


static int get(int8_t idx) {
    switch (idx) {
        case 0:
            printf("%d\n", settings.log_level);
            return 0;
        case 1:
            printf("%d\n", settings.wan_type);
            return 0;
        default:
            print_str("Unexpected variable\n");
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
    } else if (argc == 2) {
        // Set
        error = settings_set(argv[1], argv[2]);
        if (error) {
            print_str("Unexpected variable\n");
            return error;
        }
        error = settings_save();
        if (error) {
            printf("Error saving the settings file");
            return error;
        }
    } else {
        printf("unexpected number of arguments: %d\n", argc);
        return -1;
    }

    return error;
}
