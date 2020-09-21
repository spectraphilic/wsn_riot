// Standard Library
#include "stdio.h"
#include "string.h"

// RIOT
#include "fmt.h"

// Project
#include "settings.h"


struct Option {
    char *name;
    char *help;
};

struct Option options[] = {
    {
        .name = "log.level",
        .help = "0=off 1=fatal 2=error 3=warn 4=info 5=debug 6=trace",
    },
    {
        .name = "wan.type",
        .help = "0=disabled 1=4g 2=iridium",
    }
};

static const size_t len = sizeof options / sizeof options[0];

static int8_t index(const char *name)
{
    for (size_t i=0; i < len; i++) {
        if (strcmp(name, options[i].name) == 0) {
            return i;
        }
    }

    return -1;
}


static void get(int8_t idx) {
    switch (idx) {
        case 0:
            printf("%d\n", settings.log_level);
            break;
        case 1:
            printf("%d\n", settings.wan_type);
            break;
    }
}


static void set(int8_t idx, const char *value) {
    switch (idx) {
        case 0:
            settings.log_level = (log_level_t) scn_u32_dec(value, 1);
            break;
        case 1:
            settings.wan_type = (wan_type_t) scn_u32_dec(value, 1);
            break;
    }
}


extern int cmd_var(int argc, char **argv) {
    // Arguments
    if (argc > 3) {
        printf("unexpected number of arguments: %d\n", argc);
        return -1;
    }

    // Print help
    if (argc == 1) {
        for (size_t i=0; i < len; i++) {
            printf("%s %s\n", options[i].name, options[i].help);
        }
        return 0;
    }

    // Find option
    char *name = argv[1];
    int8_t idx = index(name);
    if (idx == -1) {
        print_str("Unexpected variable\n");
        return -1;
    }

    if (argc == 2) {
        get(idx);
    } else {
        set(idx, argv[2]);
    }

    return 0;
}
