#ifdef MODULE_DS18

#include "ds18.h"
#include "ds18_params.h"


/*
NOTE: 
- find out how to wire the sensor to the board, which pin to connect the data line. 
- Also, there are two modes, one that require specific hardware specs which Re-mote or wasp might not allow see the doc and simply test.

int cmd_ds18b20(int argc, char **argv) {
    ds18_t dev;
    int error = 0;

    // Arguments
    if (argc != 1) {
        printf("unexpected number of arguments: %d\n", argc);
        return -1;
    }
    assert(argv); // Avoids warning

    switch (ds18_init(&dev, &ds18_params[0])) {
        case DS18_ERROR:
            puts("[Error] The sensor pin could not be initialized");
            goto exit;
        default:
            // all good -> do nothing
            break;
    }

    int16_t temperature;
    // Get temperature in centidegrees celsius
    if (ds18_get_temperature(&dev, &temperature) == DS18_OK) {
        bool negative = (temperature < 0);
        if (negative) {
            temperature = -temperature;
        }

        printf("Temperature [ºC]: %c%d.%02d"
               "\n+-------------------------------------+\n",
               negative ? '-': ' ',
               temperature / 100,
               temperature % 100);
    }
    else {
        puts("[Error] Could not read temperature");
    }

exit:

    return error;
}
*/

#endif
