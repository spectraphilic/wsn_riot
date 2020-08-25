#define SHT3X_PARAM_I2C_ADDR    (SHT3X_I2C_ADDR_1)
#define SHT3X_PARAM_MODE        (SHT3X_SINGLE_SHOT)
#define SHT3X_PARAM_REPEAT      (SHT3X_MEDIUM)
#include "sht3x.h"
#include "sht3x_params.h"


int cmd_sht(int argc, char **argv) {
    sht3x_dev_t dev;
    int res;

    int error = 0;

    // Registers

    // Arguments
    if (argc != 1) {
        printf("unexpected number of arguments: %d\n", argc);
        return -1;
    }
    assert(argv); // Avoids warning

    if ((res = sht3x_init(&dev, &sht3x_params[0])) != SHT3X_OK) {
        puts("Initialization failed\n");
        goto exit;
    }
    else {
        puts("Initialization successful\n");
    }
    printf("\n+--------Starting Measurements--------+\n");

    // Read
    int16_t temp, hum;
    if ((res = sht3x_read(&dev, &temp, &hum)) == SHT3X_OK) {
            printf("Temperature [°C]: %d.%d\n"
                   "Relative Humidity [%%]: %d.%d\n"
                   "+-------------------------------------+\n",
                   temp / 100, temp % 100,
                   hum / 100, hum % 100);
    }
    else {
            printf("Could not read data from sensor, error %d\n", res);
    }

exit:

    return error;
}
