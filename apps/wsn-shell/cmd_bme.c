#ifdef MODULE_BME280_I2C

// RIOT
#include <fmt.h>
#include <periph/gpio.h>
#include <ztimer.h>

// Project
#include <bmx280.h>
#include <bmx280_params.h>


int cmd_bme(int argc, char **argv) {
    // Arguments
    (void)argv;
    if (check_argc(argc, 1) < 0) {
        return -1;
    }

    //const bmx280_params_t * sens_param;
    int error = 0;

#ifdef CPU_ATMEGA1281
    // This should be ifdef BOARD_WASPMOTE_PRO but it doesn't work: open issue?
    // Switch on 3v3
    error = gpio_init(SENS_PW_3V3, GPIO_OUT);
    if (error) {
        printf("[Error %d] Failed to switch on 3v3\n", error);
        return -1;
    }
    gpio_set(SENS_PW_3V3);
    ztimer_sleep(ZTIMER_MSEC, 100);
#endif

    // Initialize
    bmx280_t dev;
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

#ifdef CPU_ATMEGA1281
    gpio_clear(SENS_PW_3V3);
#endif

    return error;
}

#endif
