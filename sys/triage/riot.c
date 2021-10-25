#include <errno.h>
#include <stdio.h>


/* This is copy-paste from RIOT/cpu/avr8_common/avr_libc_extra/posix_unistd.c
 */
char* errno_string(int errnum)
{
    if (errnum < 0) {
        errnum = -errnum;
    }

    switch (errnum) {
        case 0: return "OK";
        case EDOM: return "EDOM";
        case ERANGE: return "ERANGE";
        case ENOSYS: return "ENOSYS";
        case EINTR: return "EINTR";
        case E2BIG: return "E2BIG";
        case EACCES: return "EACCES";
        case EADDRINUSE: return "EADDRINUSE";
        case EADDRNOTAVAIL: return "EADDRNOTAVAIL";
        case EAFNOSUPPORT: return "EAFNOSUPPORT";
        case EAGAIN: return "EAGAIN";
        case EALREADY: return "EALREADY";
        case EBADF: return "EBADF";
        case EBUSY: return "EBUSY";
        case ECHILD: return "ECHILD";
        case ECONNABORTED: return "ECONNABORTED";
        case ECONNREFUSED: return "ECONNREFUSED";
        case ECONNRESET: return "ECONNRESET";
        case EDEADLK: return "EDEADLK";
        case EDESTADDRREQ: return "EDESTADDRREQ";
        case EEXIST: return "EEXIST";
        case EFAULT: return "EFAULT";
        case EFBIG: return "EFBIG";
        case EHOSTUNREACH: return "EHOSTUNREACH";
        case EILSEQ: return "EILSEQ";
        case EINPROGRESS: return "EINPROGRESS";
        case EINVAL: return "EINVAL";
        case EIO: return "EIO";
        case EISCONN: return "EISCONN";
        case EISDIR: return "EISDIR";
        case ELOOP: return "ELOOP";
        case EMFILE: return "EMFILE";
        case EMLINK: return "EMLINK";
        case EMSGSIZE: return "EMSGSIZE";
        case ENAMETOOLONG: return "ENAMETOOLONG";
        case ENETDOWN: return "ENETDOWN";
        case ENETRESET: return "ENETRESET";
        case ENETUNREACH: return "ENETUNREACH";
        case ENFILE: return "ENFILE";
        case ENOBUFS: return "ENOBUFS";
        case ENODEV: return "ENODEV";
        case ENOENT: return "ENOENT";
        case ENOEXEC: return "ENOEXEC";
        case ENOLCK: return "ENOLCK";
        case ENOMEM: return "ENOMEM";
        case ENOMSG: return "ENOMSG";
        case ENOPROTOOPT: return "ENOPROTOOPT";
        case ENOSPC: return "ENOSPC";
        case ENOTCONN: return "ENOTCONN";
        case ENOTDIR: return "ENOTDIR";
        case ENOTEMPTY: return "ENOTEMPTY";
        case ENOTSOCK: return "ENOTSOCK";
        case ENOTTY: return "ENOTTY";
        case ENXIO: return "ENXIO";
        case EOPNOTSUPP: return "EOPNOTSUPP";
        case EPERM: return "EPERM";
        case EPIPE: return "EPIPE";
        case EPROTONOSUPPORT: return "EPROTONOSUPPORT";
        case EPROTOTYPE: return "EPROTOTYPE";
        case EROFS: return "EROFS";
        case ESPIPE: return "ESPIPE";
        case ESRCH: return "ESRCH";
        case ETIMEDOUT: return "ETIMEDOUT";
        //case EWOULDBLOCK: return "EWOULDBLOCK";
        case EXDEV: return "EXDEV";
        default: return "unknown";
    }
}


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
