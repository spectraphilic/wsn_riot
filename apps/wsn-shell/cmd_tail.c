#ifdef MODULE_VFS

#include <fcntl.h>

// RIOT
#include <fmt.h>
#include <log.h>
#include <tiny_strerror.h>
#include <vfs.h>

// Project
#include <triage.h>


int cmd_tail(int argc, char **argv)
{
    if (check_argc(argc, 3) < 0) {
        return -1;
    }

    uint32_t maxnl = scn_u32_dec(argv[1], 4);
    if (maxnl == 0) {
        LOG_WARNING("Unexpected number of lines");
        return -1;
    }

    // Open
    char *name = argv[2];
    int fd = vfs_open(name, O_RDONLY, 0);
    if (fd < 0) {
        LOG_ERROR("Failed to open %s (%s)", name, tiny_strerror(fd));
        return -1;
    }

    // Read backwards
    char c;
    off_t offset, nc = 0;
    uint32_t nl = 0;
    ssize_t n;

    offset = vfs_lseek(fd, 0, SEEK_END);
    while (offset > 0) {
        n = vfs_pread(fd, &c, 1, --offset);
        if (n < 1) {
            break;
        }
        if (c == '\n') {
            nl++;
            if (nl >= maxnl) {
                offset++;
                break;
            }
        }
        nc++;
    }

    printf("nl=%lu nc=%ld offset=%ld\n", nl, nc, offset);

    // Read forward
    size_t size = 255;
    char buffer[size];
    vfs_lseek(fd, offset, SEEK_SET);
    while (1) {
        n = vfs_read(fd, buffer, size - 1);
        if (n <= 0) { // Error or end-of-file
            break;
        }
        buffer[n] = '\0';
        print_str(buffer);
    }

    int error = (n < 0);

    // Close
    error = vfs_close(fd);
    return error;
}

#endif
