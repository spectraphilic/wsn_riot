#ifdef MODULE_LITTLEFS2

// RIOT
#include <board.h> /* MTD_0 should be defined in board.h */
#include <fs/littlefs2_fs.h>
#include <log.h>
#include <mtd.h>
#include <panic.h>
#include <tiny_strerror.h>
#include <vfs.h>

// Project
#include "triage.h"


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


static littlefs2_desc_t fs_desc = {
    .lock = MUTEX_INIT,
};

static vfs_mount_t mountp = {
    .fs = &littlefs2_file_system,   // Driver
    .private_data = &fs_desc,       // Driver data
    .mount_point = "/",             // Mount point
};

int wsn_mount(void)
{
    fs_desc.dev = MTD_0;

    int error = vfs_mount(&mountp);
    if (error < 0) {
        LOG_ERROR("func=wsn_mount callee=vfs_mount errno=%s", tiny_strerror(error));
        //core_panic(PANIC_GENERAL_ERROR, "SD card is required");
        return error;
    }

    LOG_INFO("SD card mounted");
    return 0;
}

int wsn_format(void)
{
    int error;

    // Do not check error. This will happen if the card has a different file
    // system
    vfs_umount(&mountp);

    error = vfs_format(&mountp);
    if (error < 0) {
        LOG_ERROR("Failed to format (%s)\n", tiny_strerror(error));
        return error;
    }

    error = vfs_mount(&mountp);
    if (error < 0) {
        LOG_ERROR("func=wsn_format callee=vfs_mount errno=%s", tiny_strerror(error));
        return error;
    }

    LOG_INFO("Successfully formatted\n");
    return 0;
}


#endif
