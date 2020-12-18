#include <errno.h>
#include <stdio.h>


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

        _case_snprintf_errno_name(ENODEV);

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


void test_utils_interactive_sync(void)
{
    char c = '\0'; /* Print help on first loop */
    do {
        if (c == 'r') {
            /* This one should have a different case than the help message
             * otherwise we match it when using 'expect' */
            puts("READY");
        }
        else if (c != '\n' && c != '\r') {
            puts("Help: Press s to start test, r to print it is ready");
        }
        c = getchar();
    } while (c != 's');

    puts("START");
}
