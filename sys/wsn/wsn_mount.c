#ifdef MODULE_FATFS_VFS

// RIOT
#include <board.h> /* MTD_0 should be defined in board.h */
#include <fs/fatfs.h>
#include <log.h>
#include <mtd.h>
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


mtd_dev_t *fatfs_mtd_devs[FF_VOLUMES];

static fatfs_desc_t fs_desc; // By default .vol_idx = 0
static vfs_mount_t mountp = {
    .fs = &fatfs_file_system,       // Driver
    .private_data = &fs_desc,       // Driver data
    .mount_point = "/",             // Mount point
};

int wsn_mount(void)
{
    fatfs_mtd_devs[fs_desc.vol_idx] = MTD_0;

    int error = vfs_mount(&mountp);
    if (error < 0) {
        char err[16];
        errno_string(error, err, sizeof(err));
        LOG_ERROR("Error mounting SD card: %s\n", err);
        return error;
    }

    LOG_INFO("SD card mounted\n");
    return 0;
}

#endif
