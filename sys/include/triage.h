/**
 * @brief Temporary home for helper functions 
 *
 * In this module we temporarily keep helper functions, until we find a better
 * place for each of them.
 */

#ifndef TRIAGE_H
#define TRIAGE_H

/**
 * @brief Print error name for the given errno
 *
 * This is copy & paste from RIOT/sys/shell/commands/sc_vfs.c because in RIOT
 * is defined as static, so we cannot use it.
 *
 * Maybe we should use strerror instead, which is standard, but it's not
 * available in AVR.
 */
int errno_string(int err, char *buf, size_t buflen);

/**
 * @brief Print formatted string to file descriptor
 *
 * Specified in POSIX.1-2008 this one should be provided by RIOT, but it is
 * not.
 */
int dprintf(int fd, const char *format, ...);

/**
 * @brief Read a line from the file descriptor
 *
 * This is like fgets except it uses a file descriptor. It's not specified in
 * POSIX as far as I know, though it could be.
 */
char* dgets(int fd, char *str, int num);


#endif
