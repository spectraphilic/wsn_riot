// Standard
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

// Posix
#include <unistd.h>

// RIOT
#include <fs/fatfs.h>
#include <log.h>
#include <mtd.h>
#include <vfs.h>


/* Macro used by errno_string to expand errno labels to string and print it */
#define _case_snprintf_errno_name(x) \
    case x: \
        res = snprintf(buf, buflen, #x); \
        break

int errno_string(int err, char *buf, size_t buflen)
{
    int len = 0;
    int res;
    if (err < 0) {
        res = snprintf(buf, buflen, "-");
        if (res < 0) {
            return res;
        }
        if ((size_t)res <= buflen) {
            buf += res;
            buflen -= res;
        }
        len += res;
        err = -err;
    }
    switch (err) {
        _case_snprintf_errno_name(EACCES);
        _case_snprintf_errno_name(ENOENT);
        _case_snprintf_errno_name(EINVAL);
        _case_snprintf_errno_name(EFAULT);
        _case_snprintf_errno_name(EROFS);
        _case_snprintf_errno_name(EIO);
        _case_snprintf_errno_name(ENAMETOOLONG);
        _case_snprintf_errno_name(EPERM);

        default:
            res = snprintf(buf, buflen, "%d", err);
            break;
    }
    if (res < 0) {
        return res;
    }
    len += res;
    return len;
}
#undef _case_snprintf_errno_name


int dprintf(int fd, const char *format, ...)
{
    int size = 255;
    char buffer[size];

    va_list args;
    va_start(args, format);
    int n = vsnprintf(buffer, size, format, args);
    va_end(args);

    if (n < 0) {
        return -1;
    }

    if (n > size - 1) { // XXX
        return -1;
    }

    return write(fd, buffer, n);
}


char* dgets(int fd, char *str, int num)
{
    char c;
    int i;
    for (i = 0; i < num; i++) {
        ssize_t n = read(fd, &c, 1);
        if (n < 0) {
            return NULL;
        } else if (n == 0) {
            break;
        }

        str[i] = c;
        if (c == '\n') {
            i++;
            break;
        }
    }

    str[i] = '\0';
    return str;
}




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

int mount_sd(void)
{
    fatfs_mtd_devs[fs_desc.vol_idx] = MTD_0;

    int error = vfs_mount(&mountp);
    if (error < 0) {
        LOG_ERROR("Error mounting SD card\n");
        return error;
    }

    LOG_INFO("SD card mounted\n");
    return 0;
}
