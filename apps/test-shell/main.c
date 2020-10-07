// Standard
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Riot
#include <log.h>
#include <shell.h>
//#include <ztimer.h>

// Project
#include "settings.h"
#include "triage.h"


extern int cmd_acc(int argc, char **argv);
extern int cmd_bme(int argc, char **argv);
extern int cmd_cat(int argc, char **argv);
extern int cmd_sht(int argc, char **argv);
extern int cmd_ds18b20(int argc, char **argv);
extern int cmd_var(int argc, char **argv);


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

    return 0;
}
*/


/*
 * Main
 */

const shell_command_t shell_commands[] = {
    {"acc", "accelerometer", cmd_acc},
    {"bme", "read BME_280", cmd_bme},
    {"cat", "print contents of given filename", cmd_cat},
    {"sht", "read SHT31", cmd_sht},
    {"var", "var [NAME [VALUE]] - type 'var' to list the variable names", cmd_var},
    //{"ds18", "read DS18B20", cmd_ds18b20},
    //{"echo", "echo", cmd_echo},
    //{"timer", "test the timer (ztimer)", cmd_timer},
    { NULL, NULL, NULL }
};


int main(void)
{
    // Print some info
    LOG_INFO("app=test-shell board=%s mcu=%s riot=%s\n", RIOT_BOARD, RIOT_MCU, RIOT_VERSION);

    // Mount the SD card
    if (mount_sd() == 0) {
        settings_load();
        LOG_INFO("Settings loaded\n");
    }

    // Run the shell
    char buffer[SHELL_DEFAULT_BUFSIZE]; // 128
    shell_run_forever(shell_commands, buffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
