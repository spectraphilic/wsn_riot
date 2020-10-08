// Standard
#include <stdio.h>
#include <stdlib.h>

// Riot
#include <log.h>
#include <shell.h>

// Project
#include "wsn.h"


extern int cmd_acc(int argc, char **argv);
extern int cmd_bme(int argc, char **argv);
extern int cmd_cat(int argc, char **argv);
extern int cmd_ds18b20(int argc, char **argv);
extern int cmd_sht(int argc, char **argv);
extern int cmd_tail(int argc, char **argv);
extern int cmd_var(int argc, char **argv);


const shell_command_t shell_commands[] = {
    {"acc", "accelerometer", cmd_acc},
    {"bme", "read BME_280", cmd_bme},
    {"cat", "print contents of given filename", cmd_cat},
    {"sht", "read SHT31", cmd_sht},
    {"tail", "print the last N lines of given filename", cmd_tail},
    {"var", "var [NAME [VALUE]] - type 'var' to list the variable names", cmd_var},
    //{"ds18", "read DS18B20", cmd_ds18b20},
    { NULL, NULL, NULL }
};


int main(void)
{
    // Boot
    LOG_INFO("app=wsn-shell board=%s mcu=%s riot=%s\n", RIOT_BOARD, RIOT_MCU, RIOT_VERSION);
    wsn_boot();

    // Run the shell
    char buffer[SHELL_DEFAULT_BUFSIZE]; // 128
    shell_run_forever(shell_commands, buffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
