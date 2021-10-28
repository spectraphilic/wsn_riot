#ifdef MODULE_VFS

// Standard
#include <stdio.h>

// Posix
#include <fcntl.h>

// Riot
#include <fmt.h>
#include <vfs.h>

// Project
#include <triage.h>


int cmd_catx(int argc, char **argv)
{
    // Arguments
    if (check_argc(argc, 2) < 0) {
        return -1;
    }
    char *name = argv[1];

    // Open
    int fd = vfs_open(name, O_RDONLY, 0);
    if (fd < 0) {
        printf("Failed to open %s (%s)\n", name, errno_string(fd));
        return -1;
    }

    // Cat
    char c;
    ssize_t n;
    while ((n = vfs_read(fd, &c, 1)) > 0) {
        print_byte_hex(c);
    }
    print_str("\n");

    vfs_close(fd);
    return 0;
}

#endif
