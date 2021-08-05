#ifdef MODULE_ARDUINO_SDI_12

// Standard
#include <stdio.h>

// RIOT
#include <log.h>

// Project
#include <qtpy.h>


int cmd_sdi(int argc, char **argv)
{

    // Arguments
    if (argc != 2) {
        LOG_WARNING("Unexpected number of arguments: %d\n", argc);
        return -1;
    }

    char *command = argv[1];
    if (command[strlen(command) - 1] != '!') {
        LOG_WARNING("Commands must end by '!'\n");
        return -1;
    }

    // Send command
    char out[100];
    qtpy_init(&dev, &qtpy_params[0]);
    qtpy_send_raw(&dev, command, out);
    qtpy_end(&dev);

    // Print answer
    printf("=> %s", out);

    return 0;
}


#endif
