#ifndef LOG_MODULE_H
#define LOG_MODULE_H

// Standard
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Riot
#include <fmt.h>


static inline const char* loglevel2str(unsigned level) {
    switch (level) {
        case LOG_NONE:
            return "[NONE   ] ";
        case LOG_ERROR:
            return "[ERROR  ] ";
        case LOG_WARNING:
            return "[WARNING] ";
        case LOG_INFO:
            return "[INFO   ] ";
        case LOG_DEBUG:
            return "[DEBUG  ] ";
        case LOG_ALL:
            return "[ALL    ] ";
        default:
            return "";
    }
}


static inline void log_write(unsigned level, const char *format, ...) {
    size_t size = 150;
    char buffer[size];

    // Timestamp + Level
    //sprintf(buffer, "%lu %s ", millis(), loglevel2str(level));
    sprintf(buffer, "%s", loglevel2str(level));

    // + Message
    size_t len = strlen(buffer);

    va_list args;
    va_start(args, format);
    vsnprintf(buffer + len, size - len - 1, format, args);
    va_end(args);

    print_str(buffer);

    // Print to file
    FILE *fp = fopen("/log2.txt", "a"); // TODO Rename to log.txt
    if (fp) {
        fputs(buffer, fp);
        fclose(fp);
    }
}

#endif
