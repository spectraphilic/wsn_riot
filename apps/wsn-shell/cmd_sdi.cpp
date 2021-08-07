#ifdef MODULE_ARDUINO_SDI_12

// Standard
#include <stdio.h>

// RIOT
#include <log.h>

// Project
#include <qtpy.h>
#include <qtpy_params.h>


extern qtpy_t qtpy_dev;

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
    qtpy_begin(&qtpy_dev);
    qtpy_send_raw(&qtpy_dev, command);
    qtpy_end(&qtpy_dev);

    // Print answer
    printf("=> %s\n", qtpy_dev.out);

    return 0;
}


#endif
