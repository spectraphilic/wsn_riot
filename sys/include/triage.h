/**
 * @defgroup triage Triage
 * @brief Temporary home for helper functions 
 *
 * In this module we temporarily keep helper functions, until we find a better
 * place for each of them.
 *
 * @{
 *
 * @file
 */

#ifndef TRIAGE_H
#define TRIAGE_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return short name for the given error number
 *
 * Where the error number is as defined in errno.h from the C standard
 * library.
 *
 * @note TODO We should use strerror instead, which is standard, but it doesn't
 *       seem to work as defined, at least with the remote-revb.
 *
 * @param[in]   errnum  the error number, see errno.h from libc
 *
 * @return      char*   string with the short name of the error number
 * @return      unknown for unexpected values of errnum
 */
char* errno_string(int errnum);


/**
 * @brief Print formatted string to file descriptor
 *
 * @note Specified in POSIX.1-2008 this one should be provided by RIOT, but it
 *       is not.
 *
 * @note We cannot use the higher level fprintf because AVR libc doesn't
 *       support it.
 *
 * @param[in]   fd      file descriptor
 * @param[in]   format  format string
 * @param[in]   ...     values to replace the specifiers in the format string
 *
 * @return      >= 0    total number of bytes written to the file descriptor
 * @return      < 0     inidicates an error
 */
int dprintf(int fd, const char *format, ...);

/**
 * @brief Read a line from the given file descriptor
 *
 * Read a line from the given file descriptor, where a line ends with a newline
 * character or the end of file.
 *
 * But read no more than the given number minus one, for the terminating null
 * character that will be written.
 *
 * @note This is like fgets except it uses a file descriptor. It's not
 *       specified in POSIX.
 *
 * @param[in]   fd      file descriptor
 * @param[out]  str     pointer to a buffer where the line will be stored
 * @param[in]   num     maximum number of characters to write, including the
 *                      terminating null character
 *
 * @return      str     the same pointer to the buffer is returned on success
 * @return      NULL    indicates an error
 */
char* dgets(int fd, char *str, int num);


/**
 * @brief Read from file description but do not change file offset
 *
 * Reads up to count bytes from file descriptor fd at offset offset (from the
 * start of the file) into the buffer starting at buf.  The file offset is not
 * changed.
 *
 * @param[in]   fd      file descriptor
 * @param[out]  buf     pointer to a buffer where the data will be stored
 * @param[in]   count   maximum number of bytes to read
 * @param[in]   offset  offset to read at
 *
 * @return      > 0 number of bytes read
 * @return      = 0 end of file
 * @return      < 0 if error
 */

ssize_t pread(int fd, void *buf, size_t count, off_t offset);


/**
 * @brief Check number of arguments
 *
 * Helper used in shell commands
 */
int check_argc(int argc, int expected);

#ifdef __cplusplus
}
#endif


#endif
/** @} */
