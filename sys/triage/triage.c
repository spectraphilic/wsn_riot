// Standard
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

// Posix
#include <unistd.h>


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
