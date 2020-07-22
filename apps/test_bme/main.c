#include <assert.h>
#include <stdio.h>

// RIOT
#include "periph/i2c.h"
#include "shell.h"
//#include "ztimer.h"

// WSN
#include "bmx280.h"


static int cmd_bme(int argc, char **argv) {
    const bmx280_t dev = I2C_DEV(0);
    const bmx280_params_t sens_param;
    int error = 0;

    // Registers

    // Arguments
    if (argc != 1) {
        printf("unexpected number of arguments: %d\n", argc);
        return -1;
    }

    // Initialize
    error = bmx280_init(dev, sens_param);
    if (error) {
        printf("bme280 check error\n");
        goto exit;
    }

    // Read
    int16_t temperature, humidity, pressure;
    temperature = bmx280_read_temperature(dev);
    humidity = bme280_read_humidity(dev);
    pressure = bmx280_read_pressure(dev);

    printf("bme280 temperature=%d humidity=%d pressure=%d\n", temperature, humidity, pressure);

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
    {"bme", "BME_280", cmd_bme},
    //{"timer", "test the timer (ztimer)", cmd_timer},
    { NULL, NULL, NULL }
};


int main(void)
{
    // Print some info
    puts("app: test-bme");
    printf("Board=%s MCU=%s\n", RIOT_BOARD, RIOT_MCU);

    // Run the shell
    char buffer[SHELL_DEFAULT_BUFSIZE]; // 128
    shell_run_forever(shell_commands, buffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
