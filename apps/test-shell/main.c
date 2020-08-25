#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// RIOT
//#include "progmem.h"
#include "shell.h"
//#include "ztimer.h"

#include "fs/fatfs.h"
#include "mtd.h"
#include "vfs.h"

#include "ds18.h"
#include "ds18_params.h"


extern int cmd_acc(int argc, char **argv);
extern int cmd_bme(int argc, char **argv);
extern int cmd_sht(int argc, char **argv);
extern int cmd_ds18b20(int argc, char **argv);


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



/*
 * Filesystem
 */

#include "board.h" /* MTD_0 should be defined in board.h */

/* Configure MTD device for SD card if none is provided */
#if !defined(MTD_0) && MODULE_MTD_SDCARD
#include "mtd_sdcard.h"
#include "sdcard_spi.h"
#include "sdcard_spi_params.h"

#define SDCARD_SPI_NUM ARRAY_SIZE(sdcard_spi_params)

/* SD card devices are provided by auto_init_sdcard_spi */
extern sdcard_spi_t sdcard_spi_devs[SDCARD_SPI_NUM];

/* Configure MTD device for the first SD card */
static mtd_sdcard_t mtd_sdcard_dev = {
    .base = {
        .driver = &mtd_sdcard_driver
    },
    .sd_card = &sdcard_spi_devs[0],
    .params = &sdcard_spi_params[0],
};
static mtd_dev_t *mtd0 = (mtd_dev_t*)&mtd_sdcard_dev;
#define MTD_0 mtd0
#endif


mtd_dev_t *fatfs_mtd_devs[FF_VOLUMES];

static fatfs_desc_t fs_desc; // By default .vol_idx = 0
static vfs_mount_t mountp = {
    .fs = &fatfs_file_system,       // Driver
    .private_data = &fs_desc,       // Driver data
    .mount_point = "/",             // Mount point
};



/*
 * Main
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

    // Mount the SD card
    fatfs_mtd_devs[fs_desc.vol_idx] = MTD_0;
    int error = vfs_mount(&mountp);
    if (error) {
        printf("Error mounting SD card\n");
    } else {
        printf("SD card mounted\n");
    }

    // Run the shell
    char buffer[SHELL_DEFAULT_BUFSIZE]; // 128
    shell_run_forever(shell_commands, buffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
