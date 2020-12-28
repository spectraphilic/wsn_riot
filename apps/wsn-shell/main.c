// Standard
#include <board.h>
#include <stdio.h>
#include <stdlib.h>

// Riot
#include <log.h>
#include <shell.h>

// Project
#include "triage.h"
#include "wsn.h"


extern int cmd_acc(int argc, char **argv);
extern int cmd_bme(int argc, char **argv);
extern int cmd_cat(int argc, char **argv);
extern int cmd_ds18b20(int argc, char **argv);
extern int cmd_sht(int argc, char **argv);
extern int cmd_tail(int argc, char **argv);
extern int cmd_var(int argc, char **argv);


const shell_command_t shell_commands[] = {
#ifdef MODULE_LIS3331LDH
    {"acc", "accelerometer", cmd_acc},
#endif
#ifdef MODULE_BME280_I2C
    {"bme", "read BME_280", cmd_bme},
#endif
#ifdef MODULE_VFS
    {"cat", "print contents of given filename", cmd_cat},
#endif
#ifdef MODULE_SHT3X
    {"sht", "read SHT31", cmd_sht},
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
    test_utils_interactive_sync();

    // Boot
#ifdef MODULE_WSN
    wsn_boot();
#endif
    LOG_INFO("app=wsn-shell board=%s mcu=%s riot=%s\n", RIOT_BOARD, RIOT_MCU, RIOT_VERSION);

    // Run the shell
    char buffer[SHELL_DEFAULT_BUFSIZE]; // 128
    shell_run_forever(shell_commands, buffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
