#ifndef LOG_MODULE_H
#define LOG_MODULE_H

#include <stdarg.h>
#include <stdio.h>


static inline void log_write(unsigned level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    switch (level) {
        case LOG_NONE:
            printf("[NONE   ] ");
            break;
        case LOG_ERROR:
            printf("[ERROR  ] ");
            break;
        case LOG_WARNING:
            printf("[WARNING] ");
            break;
        case LOG_INFO:
            printf("[INFO   ] ");
            break;
        case LOG_DEBUG:
            printf("[DEBUG  ] ");
            break;
        case LOG_ALL:
            printf("[ALL    ] ");
            break;
    }
    vprintf(format, args);
    va_end(args);
}

#endif
