#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// RIOT
#include "periph/i2c.h"
//#include "progmem.h"
#include "shell.h"
//#include "ztimer.h"

// WSN Sensors
#include "lis3331ldh.h"
#include "bmx280.h"
#include "bmx280_params.h"
#include "fmt.h"


#define SHT3X_PARAM_I2C_ADDR    (SHT3X_I2C_ADDR_1)
#define SHT3X_PARAM_MODE        (SHT3X_SINGLE_SHOT)
#define SHT3X_PARAM_REPEAT      (SHT3X_MEDIUM)
#include "sht3x.h"
#include "sht3x_params.h"

#include "ds18.h"
#include "ds18_params.h"


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


/*
NOTE: 
- find out how to wire the sensor to the board, which pin to connect the data line. 
- Also, there are two modes, one that require specific hardware specs which Re-mote or wasp might not allow see the doc and simply test.

static int cmd_ds18b20(int argc, char **argv) {
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

    /*
    NOTES (July 27, 2020 - Simon):    
    The calibration is needed for the temperature reading to work. 
    Not too sure why at the moment. Hum and pressure do not require 
    to readout the calib values. Is this needed at each measurment 
    or only once at start up? 

    */
    puts("Initialization successful\n");

    puts("+------------Calibration Data------------+");
    printf("dig_T1: %u\n", dev.calibration.dig_T1);
    printf("dig_T2: %i\n", dev.calibration.dig_T2);
    printf("dig_T3: %i\n", dev.calibration.dig_T3);

    printf("dig_P1: %u\n", dev.calibration.dig_P1);
    printf("dig_P2: %i\n", dev.calibration.dig_P2);
    printf("dig_P3: %i\n", dev.calibration.dig_P3);
    printf("dig_P4: %i\n", dev.calibration.dig_P4);
    printf("dig_P5: %i\n", dev.calibration.dig_P5);
    printf("dig_P6: %i\n", dev.calibration.dig_P6);
    printf("dig_P7: %i\n", dev.calibration.dig_P7);
    printf("dig_P8: %i\n", dev.calibration.dig_P8);
    printf("dig_P9: %i\n", dev.calibration.dig_P9);

    printf("dig_H1: %u\n", dev.calibration.dig_H1);
    printf("dig_H2: %i\n", dev.calibration.dig_H2);
    printf("dig_H3: %i\n", dev.calibration.dig_H3);
    printf("dig_H4: %i\n", dev.calibration.dig_H4);
    printf("dig_H5: %i\n", dev.calibration.dig_H5);
    printf("dig_H6: %i\n", dev.calibration.dig_H6);
    puts("\n+--------Starting Measurements--------+");
    // Read
    int16_t temperature;
    uint16_t humidity;
    uint32_t pressure;
    temperature = bmx280_read_temperature(&dev);
    humidity = bme280_read_humidity(&dev);
    pressure = bmx280_read_pressure(&dev);
    
    char str_temp[8];
    size_t len = fmt_s16_dfp(str_temp, temperature, -2);
    str_temp[len] = '\0';
    char str_hum[8];
    len = fmt_s16_dfp(str_hum, humidity, -2);
    str_hum[len] = '\0';

    printf("Temperature [°C]: %s\n", str_temp);
    printf("   Pressure [Pa]: %" PRIu32 "\n", pressure);
    printf("  Humidity [%%rH]: %s\n", str_hum);
    //printf("bme280 temperature=%d humidity=%u pressure=%lu\n", temperature, humidity, pressure);

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
static int cmd_echo(int argc, char **argv) {
    for (int i=0; i < argc; i++) {
        printf("argv[%d]=%s\n", i, argv[i]);
    }

    printf("%S\n", PSTR("YES"));
    printf("%S\n", PSTR("0123456789"));

    return 0;
}
*/

const shell_command_t shell_commands[] = {
    {"acc", "accelerometer", cmd_acc},
    {"bme", "read BME_280", cmd_bme},
    {"sht", "read SHT31", cmd_sht},
    //{"ds18", "read DS18B20", cmd_ds18b20},
    //{"echo", "echo", cmd_echo},
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
