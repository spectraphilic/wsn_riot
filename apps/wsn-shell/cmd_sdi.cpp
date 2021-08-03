#ifdef MODULE_ARDUINO_SDI_12

// Standard
#include <stdio.h>

// RIOT
#include <log.h>
#include <SDI12.h>


#ifndef SDI12_DATA_PIN
#define SDI12_DATA_PIN 13
#endif


int cmd_sdi(int argc, char **argv) {
    SDI12 sdi12(SDI12_DATA_PIN);

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

    // Begin
    sdi12.begin();
    delay(500);  // allow things to settle

    // Send command
    sdi12.sendCommand(command);
    printf("=> ");
    delay(300);                  // wait a while for a response
    while (sdi12.available())    // write the response to the screen
        putchar((char) sdi12.read());

    // Done
    sdi12.end();
    return 0;
}


#endif
