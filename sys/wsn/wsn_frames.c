#include <stdio.h>
#include <time.h>

#include <fcntl.h>

#include <vfs.h>

#include <queue.h>
#include <triage.h>


typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint32_t offset;
    uint8_t size;
} item_t;


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
    item_t item = {year, month, day, offset, size};
    queue_push(&item);

    return 0;
}

int wsn_load_frame(uint8_t *data, uint8_t *size)
{
    char filename[30];
    item_t item;

    uint32_t n = queue_peek(&item);
    if (n == 0) {
        return 0;
    }

    get_data_filename(filename, item.year, item.month, item.day);
    int fd = vfs_open(filename, O_RDONLY, 0);
    vfs_lseek(fd, item.offset, SEEK_SET);
    *size = vfs_read(fd, data, item.size);
    vfs_close(fd);

    return n;
}

int wsn_drop_frame(void)
{
    return queue_drop();
}
