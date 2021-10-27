#include <errno.h>
#include <fcntl.h>

// Riot
#include <log.h>
#include <vfs.h>

// Project
#include "queue.h"
#include "triage.h"


const uint32_t headsize = 4;

static queue_t queue = {
    .path = "/QUEUE.BIN",
    .itemsize = 8
};


static int write_header(int fd, queue_header_t header)
{
    vfs_lseek(fd, 0, SEEK_SET);
    ssize_t size = vfs_write(fd, &header, sizeof(header));
    if (size < 0) {
        LOG_ERROR("Failed to write header (%s)", errno_string(size));
        return -1;
    }
    return 0;
}

int queue_make(void)
{
    // Create file
    int fd = vfs_open(queue.path, O_CREAT | O_WRONLY | O_SYNC | O_EXCL, 0);
    if (fd == -EEXIST) {
        return 0;
    }
    if (fd < 0) {
        LOG_ERROR("Failed to open %s (%s)", queue.path, errno_string(fd));
        return fd;
    }

    // Header
    queue_header_t header;
    header.offset = sizeof(header); // the first element starts just after the header

    // Write header
    int error = write_header(fd, header);
    if (error) {
        vfs_close(fd);
        return error;
    }

    // Close
    return vfs_close(fd);
}

int queue_push(void *item)
{
    int fd = vfs_open(queue.path, O_WRONLY | O_APPEND | O_SYNC, 0);
    vfs_write(fd, item, queue.itemsize);
    vfs_close(fd);

    return 0;
}

int queue_peek(void *item)
{
    queue_header_t header;

    int fd = vfs_open(queue.path, O_RDONLY, 0);
    vfs_read(fd, &header, sizeof(header));                    // Read header
    off_t filesize = vfs_lseek(fd, 0, SEEK_END);              // File size
    uint32_t n = (filesize - header.offset) / queue.itemsize; // Number of elements

    if (n > 0) {
        vfs_lseek(fd, header.offset, SEEK_SET);
        vfs_read(fd, item, queue.itemsize);
    }

    vfs_close(fd);

    return n;
}

int queue_drop(void)
{
    queue_header_t header;

    int fd = vfs_open(queue.path, O_RDWR, 0);
    vfs_read(fd, &header, sizeof(header));                    // Read header
    off_t filesize = vfs_lseek(fd, 0, SEEK_END);              // File size
    uint32_t n = (filesize - header.offset) / queue.itemsize; // Number of elements

    if (n == 0) {
        vfs_close(fd);
        LOG_WARNING("Queue is empty");
        return -1;
    }

    header.offset += queue.itemsize;
    int error = write_header(fd, header);
    if (error) {
        vfs_close(fd);
        return error;
    }

    vfs_close(fd);
    return n - 1;
}
