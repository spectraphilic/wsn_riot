#ifdef MODULE_VFS

// Standard library
#include <stdarg.h>
#include <stdio.h>

// Posix
#include <unistd.h>

// Riot
#include <vfs.h>


char* vfs_gets(int fd, char *str, int num)
{
    char c;
    int i;
    for (i = 0; i < num; i++) {
        ssize_t n = vfs_read(fd, &c, 1);
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


int vfs_printf(int fd, const char *format, ...)
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

    return vfs_write(fd, buffer, n);
}


ssize_t vfs_pread(int fd, void *buf, size_t count, off_t offset)
{
    off_t position = vfs_lseek(fd, 0, SEEK_CUR);
    if (position < 0) {
        return position;
    }

    vfs_lseek(fd, offset, SEEK_SET);
    ssize_t n = vfs_read(fd, buf, count);
    vfs_lseek(fd, position, SEEK_SET);

    return n;
}

#endif
