// Standard
#include <stdlib.h>

// Riot
#include <log.h>
#include <shell.h>

// Project
#include <wsn.h>


static int check_argc(int argc, int expected)
{
    if (argc != expected) {
        LOG_WARNING("Expected %d arguments, got %d", expected, argc);
        return -1;
    }

    return 0;
}

static int cmd_time(int argc, char **argv)
{
    if (check_argc(argc, 2) < 0) {
        return -1;
    }

    unsigned long time = strtoul(argv[1], NULL, 10);
    return wsn_time_set(time);
}

static int cmd_ack(int argc, char **argv)
{
    (void)argv;
    if (check_argc(argc, 1) < 0) {
        return -1;
    }

    int n = wsn_drop_frame();
    if (n < 0) {
        LOG_ERROR("Failed to drop frame");
        return -1;
    }

    return 0;
}

const shell_command_t shell_commands[] = {
    {"time", "set time", cmd_time},
    {"ack",  "", cmd_ack},
};
