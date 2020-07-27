#include <assert.h>
#include <stdio.h>

// RIOT
#include "periph/i2c.h"
#include "shell.h"
//#include "ztimer.h"

// WSN Sensors
#include "lis3331ldh.h"
#include "bmx280.h"
#include "bmx280_params.h"
#include "sht3x.h"
#include "sht3x_params.h"


static int cmd_acc(int argc, char **argv) {
    const i2c_t dev = I2C_DEV(0);
    int error = 0;

    // Registers

    // Arguments
    if (argc != 1) {
        printf("unexpected number of arguments: %d\n", argc);
        return -1;
    }
    assert(argv); // Avoids warning

    // Acquire
    error = i2c_acquire(dev);
    if (error)
    {
        printf("i2c_acquire error=%d\n", error);
        return error;
    }

    // Initialize
    error = lis3331ldh_init(dev);
    if (error) {
        printf("ACC check error\n");
        goto exit;
    }

    // Read
    int16_t x, y, z;
    error = lis3331ldh_read_xyz(dev, &x, &y, &z);
    if (error) {
        printf("ACC Error\n");
        goto exit;
    }

    printf("ACC x=%d y=%d z=%d\n", x, y, z);

exit:
    // Release
    i2c_release(dev);
    return error;
}

static int cmd_bme(int argc, char **argv) {
    bmx280_t dev;
    //const bmx280_params_t * sens_param;
    int error = 0;

    // Registers

    // Arguments
    if (argc != 1) {
        printf("unexpected number of arguments: %d\n", argc);
        return -1;
    }
    assert(argv); // Avoids warning

    // Initialize
    switch (bmx280_init(&dev, &bmx280_params[0])) {
        case BMX280_ERR_BUS:
            puts("[Error] Something went wrong when using the I2C bus");
            goto exit;
        case BMX280_ERR_NODEV:
            puts("[Error] Unable to communicate with any BMX280 device");
            goto exit;
        default:
            /* all good -> do nothing */
            break;
    }

    // Read
    int16_t temperature;
    uint16_t humidity;
    uint32_t pressure;
    temperature = bmx280_read_temperature(&dev);
    humidity = bme280_read_humidity(&dev);
    pressure = bmx280_read_pressure(&dev);

    printf("bme280 temperature=%d humidity=%u pressure=%lu\n", temperature, humidity, pressure);

exit:

    return error;
}


static int cmd_sht(int argc, char **argv) {
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





/*
static int cmd_timer(int argc, char **argv) {
    assert(argc);
    assert(argv);

    printf("now = %lu\n", ztimer_now(ZTIMER_MSEC));
    ztimer_sleep(ZTIMER_MSEC, 5000);
    printf("now = %lu\n", ztimer_now(ZTIMER_MSEC));

    return 0;
}
*/

/*
static int echo(int argc, char **argv) {
    for (int i=0; i < argc; i++) {
        printf("argv[%d]=%s\n", i, argv[i]);
    }

    return 0;
}
*/

const shell_command_t shell_commands[] = {
    //{"acc", "accelerometer", cmd_acc},
    {"bme", "read BME_280", cmd_bme},
    {"sht", "read SHT31", cmd_sht},
    //{"timer", "test the timer (ztimer)", cmd_timer},
    { NULL, NULL, NULL }
};


int main(void)
{
    // Print some info
    puts("app: test-shell");
    printf("Board=%s MCU=%s\n", RIOT_BOARD, RIOT_MCU);

    // Run the shell
    char buffer[SHELL_DEFAULT_BUFSIZE]; // 128
    shell_run_forever(shell_commands, buffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
