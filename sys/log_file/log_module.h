#ifndef LOG_MODULE_H
#define LOG_MODULE_H

#ifdef __cplusplus
/* restrict is a C99 keyword, not valid in C++, but GCC and Clang have the
 * __restrict__ extension keyword which can be used instead */
#define restrict __restrict__
#endif

// Standard
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Posix
#include <fcntl.h>

// Riot
#include <fmt.h>
#include <thread.h>
#include <timex.h>
#include <ztimer.h>
#include <vfs.h>

// Project
#include <triage.h>
#include <wsn.h>


static inline const char* loglevel2str(unsigned level) {
    switch (level) {
        case LOG_NONE:
            return "none";
        case LOG_ERROR:
            return "error";
        case LOG_WARNING:
            return "warning";
        case LOG_INFO:
            return "info";
        case LOG_DEBUG:
            return "debug";
        case LOG_ALL:
            return "all";
        default:
            return "";
    }
}


static inline void log_write(unsigned level, const char *format, ...) {
    size_t buffer_len = 180;
    char buffer[buffer_len];
    char message[80];

    // Message
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message) - 1, format, args);
    va_end(args);

    // Log line
    unsigned ms;
    ztimer_now_t now = wsn_time_get(&ms);
    snprintf(
        buffer,
        buffer_len,
        "time=%lu.%03u level=%s thread=%s %s",
        now,
        ms,
        loglevel2str(level),
        thread_getname(thread_getpid()),
        message
    );

    // Append a new line if there is not one already
    size_t n = strlen(buffer);
    if (buffer[n-1] != '\n') {
        if (n == buffer_len - 1)
            n--;

        buffer[n] = '\n';
        buffer[n+1] = '\0';
    }

    // Print to stdout
    print_str(buffer);

    // Print to file
    const char *filename = "/log.txt";
    int fd = vfs_open(filename, O_CREAT | O_WRONLY | O_APPEND, 0);
    if (fd >= 0) {
        int len = strlen(buffer);
        ssize_t size = vfs_write(fd, buffer, len);
        if (size < 0) {
            printf("Error callee=vfs_write args=%s errno=%s\n", filename, errno_string(size));
        }
        else if (size != (ssize_t)len) {
            printf("Error callee=vfs_write args=%s wrote %d bytes should be %d\n", filename, size, len);
        }
        vfs_close(fd);
    }
}

#endif
