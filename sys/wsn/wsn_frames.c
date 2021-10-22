#include <stdio.h>
#include <time.h>

#include <fcntl.h>

#include <vfs.h>

#include <queue.h>
#include <triage.h>


static int get_data_filename(char *filename, int year, int month, int day)
{
    return sprintf(filename, "data/%d%d%d.bin", year, month, day);
}


int wsn_save_frame(time_t time, const void *data, uint8_t size)
{
    char filename[30];

    // Get filename
    struct tm *calendar = gmtime(&time);
    int year = calendar->tm_year % 100; // Since 1900, but we only care about the last 2 digits
    int month = calendar->tm_mon + 1; // Starts from zero, so we add +1
    int day = calendar->tm_mday;
    get_data_filename(filename, year, month, day);

    // Save frame
    int fd = vfs_open(filename, O_CREAT | O_WRONLY | O_APPEND | O_SYNC, 0);
    if (fd < 0) {
        return fd;
    }
    off_t offset = tell(fd);
    vfs_write(fd, data, size);
    vfs_close(fd);

    // Add to queue
    uint8_t item[8] = {year, month, day};
    *(uint32_t *)(item + 3) = offset;
    item[7] = size;
    queue_push(item);

    return 0;
}


