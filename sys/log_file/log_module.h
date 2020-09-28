#ifndef LOG_MODULE_H
#define LOG_MODULE_H

// Standard
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Posix
#include <fcntl.h>
#include <unistd.h>

// Riot
#include <fmt.h>
#include <ztimer.h>


static inline const char* loglevel2str(unsigned level) {
    switch (level) {
        case LOG_NONE:
            return "NONE    ";
        case LOG_ERROR:
            return "ERROR   ";
        case LOG_WARNING:
            return "WARNING ";
        case LOG_INFO:
            return "INFO    ";
        case LOG_DEBUG:
            return "DEBUG   ";
        case LOG_ALL:
            return "ALL     ";
        default:
            return "";
    }
}


static inline void log_write(unsigned level, const char *format, ...) {
    size_t size = 150;
    char buffer[size];

    // Timestamp + Level
    ztimer_now_t now = ztimer_now(ZTIMER_USEC);
    unsigned seconds = now / 1000000;
    unsigned ms = (now / 1000) % 1000;
    sprintf(buffer, "%u.%03u %s", seconds, ms, loglevel2str(level));

    // + Message
    size_t len = strlen(buffer);

    va_list args;
    va_start(args, format);
    vsnprintf(buffer + len, size - len - 1, format, args);
    va_end(args);

    print_str(buffer);

    // Print to file
    int fd = open("/log2.txt", O_APPEND | O_CREAT); // TODO Rename to log.txt
    if (fd >= 0) {
        write(fd, buffer, strlen(buffer));
        close(fd);
    }
}

#endif
