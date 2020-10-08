// Standard
#include <errno.h>

// Posix
#include <fcntl.h>
#include <unistd.h>

// Project
#include <log.h>
#include <triage.h>


int cmd_cat(int argc, char **argv)
{
    if (argc != 2) {
        LOG_WARNING("Unexpected number of arguments: %d\n", argc);
        return -1;
    }

    char *name = argv[1];

    // Open
    int fd = open(name, O_RDONLY);
    if (fd < 0) {
        char err[16];
        errno_string(errno, err, sizeof(err));
        LOG_ERROR("Failed to open %s (%s)\n", name, err);
        return -1;
    }

    // Cat
    char c;
    ssize_t n;
    while ((n = read(fd, &c, 1)) > 0) {
        putchar(c);
    }

    int error = (n < 0);

    // Close
    error = close(fd);
    return error;
}
