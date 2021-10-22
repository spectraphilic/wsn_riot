#include <errno.h>
#include <fcntl.h>

#include <log.h>
#include <vfs.h>

#include "queue.h"


static queue_t queue = {
    .path = "/QUEUE.BIN",
    .itemsize = 8
};

int queue_make(void)
{
    // Create file
    int fd = vfs_open(queue.path, O_CREAT | O_WRONLY | O_SYNC | O_EXCL, 0);
    if (fd == -EEXIST) {
        return 0;
    }
    if (fd < 0) {
        return fd;
    }

    // Write header
    uint32_t first = 0;
    ssize_t n = vfs_write(fd, &first, 4);
    if (n < 0) {
        vfs_close(fd);
        return n;
    }

    // Close
    return vfs_close(fd);
}

int queue_push(void *item)
{
    int fd = vfs_open(queue.path, O_CREAT | O_WRONLY | O_APPEND | O_SYNC, 0);
    vfs_write(fd, item, queue.itemsize);
    vfs_close(fd);

    return 0;
}
