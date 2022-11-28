// Standard
#include <stdio.h>
#include <stdlib.h>

// Riot
#include <board.h>
#include <log.h>
#include <shell.h>

// Project
#include <wsn.h>
#include "commands.h"
#include "commands.hpp"


const shell_command_t shell_commands[] = {
#ifdef MODULE_LIS3331LDH
    {"acc", "accelerometer", cmd_acc},
#endif
#ifdef MODULE_BME280_I2C
    {"bme", "read BME_280", cmd_bme},
#endif
#ifdef MODULE_VFS
    {"cat", "print contents of given filename", cmd_cat},
    {"catx", "print contents of given filename in hexadecimal", cmd_catx},
    {"format", "format the SD card", cmd_format},
#endif
    {"gpio", "handle digital pins", cmd_gpio},
#ifdef MODULE_GPS
    {"gps", "print data from GPS", cmd_gps},
#endif
#ifdef MODULE_DS3231
    {"rtc_temp", "get temperature", cmd_rtc_temp},
#endif
#if defined(MODULE_DS1307) || defined(MODULE_DS3231)
    {"rtc_time", "get/set time", cmd_rtc_time},
#endif
#ifdef MODULE_SHT3X
    {"sht", "read SHT31", cmd_sht},
#endif
#ifdef MODULE_ARDUINO_SDI_12
    {"sdi", "send SDI-12 commands", cmd_sdi},
#endif
#ifdef MODULE_VFS
    {"tail", "print the last N lines of given filename", cmd_tail},
#endif
#ifdef MODULE_SETTINGS
    {"var", "var [NAME [VALUE]] - type 'var' to list the variable names", cmd_var},
#endif
#ifdef MODULE_DS18
    {"ds18", "read DS18B20", cmd_ds18b20},
#endif
    { NULL, NULL, NULL }
};


int main(void)
{
    LED0_ON;

    // Boot
#ifdef MODULE_WSN
    wsn_boot();
#endif
    LOG_INFO("app=wsn-shell board=%s mcu=%s riot=%s", RIOT_BOARD, RIOT_MCU, RIOT_VERSION);

    // Run the shell
    char buffer[SHELL_DEFAULT_BUFSIZE]; // 128
    shell_run_forever(shell_commands, buffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
