// Standard
#include <errno.h>
#include <stdio.h>
#include <time.h>

// Posix
#include <fcntl.h>

// Riot
#include <log.h>
#include <vfs.h>

// Project
#include <triage.h>


typedef struct {
    uint32_t offset;
} queue_header_t;

typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint32_t offset;
    uint8_t size;
} item_t;

const char queue_path[] = "/queue.bin";
const uint8_t head_size = sizeof(queue_header_t);
const uint8_t item_size = sizeof(item_t);


static int write_header(int fd, queue_header_t *header)
{
    vfs_lseek(fd, 0, SEEK_SET);
    ssize_t size = vfs_write(fd, header, head_size);
    if (size < 0) {
        LOG_ERROR("Failed to write header (%s)", errno_string(size));
        return -1;
    }
    return 0;
}

static uint32_t read_header(int fd, queue_header_t *header)
{
    // file offset must be zero
    vfs_read(fd, header, head_size);             // Read header
    off_t filesize = vfs_lseek(fd, 0, SEEK_END);  // File size
    uint32_t n = (filesize - header->offset) / item_size; // Number of elements
    return n;
}

static int queue_make(void)
{
    // Create file
    int fd = vfs_open(queue_path, O_CREAT | O_WRONLY | O_SYNC | O_EXCL, 0);
    if (fd == -EEXIST) {
        return 0;
    }
    if (fd < 0) {
        LOG_ERROR("Failed to open %s (%s)", queue_path, errno_string(fd));
        return fd;
    }

    // Header
    queue_header_t header;
    header.offset = head_size; // the first element starts just after the header

    // Write header
    int error = write_header(fd, &header);
    if (error) {
        vfs_close(fd);
        return error;
    }

    // Close
    return vfs_close(fd);
}

static int queue_push(void *item)
{
    int fd = vfs_open(queue_path, O_WRONLY | O_APPEND | O_SYNC, 0);
    if (fd < 0) {
        return fd;
    }

    ssize_t size = vfs_write(fd, item, item_size);
    vfs_close(fd);

    if (size < 0) {
        return fd;
    }

    return 0;
}

static int queue_peek(void *item)
{
    queue_header_t header;

    int fd = vfs_open(queue_path, O_RDONLY, 0);
    uint32_t n = read_header(fd, &header);

    if (n > 0) {
        vfs_lseek(fd, header.offset, SEEK_SET);
        vfs_read(fd, item, item_size);
    }

    vfs_close(fd);

    return n;
}

/*
static int queue_peek_end(void *item)
{
    queue_header_t header;

    int fd = vfs_open(queue_path, O_RDONLY, 0);
    uint32_t n = read_header(fd, &header);

    if (n > 0) {
        vfs_lseek(fd, - item_size, SEEK_END);
        vfs_read(fd, item, item_size);
    }

    vfs_close(fd);

    return n;
}
*/

static int queue_drop(void)
{
    queue_header_t header;

    int fd = vfs_open(queue_path, O_RDWR, 0);
    uint32_t n = read_header(fd, &header);

    if (n == 0) {
        vfs_close(fd);
        LOG_WARNING("Queue is empty");
        return -1;
    }

    header.offset += item_size;
    int error = write_header(fd, &header);
    if (error) {
        vfs_close(fd);
        return error;
    }

    vfs_close(fd);
    return n - 1;
}

static int get_data_filename(char *filename, int year, int month, int day)
{
    return sprintf(filename, "/data/%02d%02d%02d.bin", year, month, day);
}


/*
 * Public interface
 */

int frames_init(void)
{
    queue_make();
    int error = vfs_mkdir("/data", 0);
    if (error == -EEXIST)
        error = 0;

    return error;
}

int frames_save(time_t time, const void *data, uint8_t size)
{
    char filename[30];

    // Get filename
    struct tm *calendar = gmtime(&time);
    int year = calendar->tm_year % 100; // Since 1900, but we only care about the last 2 digits
    int month = calendar->tm_mon + 1; // Starts from zero, so we add +1
    int day = calendar->tm_mday;
    get_data_filename(filename, year, month, day);

    // Save frame
    int fd = vfs_open(filename, O_CREAT | O_WRONLY | O_SYNC | O_APPEND, 0);
    if (fd < 0) {
        LOG_ERROR("Failed to open %s (%s)", filename, errno_string(fd));
        return fd;
    }
    off_t offset = tell(fd);
    vfs_write(fd, data, size);
    vfs_close(fd);

    // Add to queue
    item_t item = {
        .year = year,
        .month = month,
        .day = day,
        .offset = offset,
        .size = size
    };
    queue_push(&item);

    return 0;
}

int frames_load(uint8_t *data, uint8_t *size)
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

int frames_drop(void)
{
    return queue_drop();
}
