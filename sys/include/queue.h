/**
 * @defgroup wsn Wireless Sensor Network
 * @brief Queue
 *
 * TODO Write longer description
 *
 * @{
 *
 * @file
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*/
 * @brief Header of a queue
 */

typedef struct {
    uint32_t offset;
} queue_header_t;

/**
 * @brief Description of a queue
 */

typedef struct {
    char *path;
    uint8_t itemsize;
} queue_t;


/**
 * @brief Creates the queue on-disk
 */
int queue_make(void);

/**
 * @brief Appends a new item to the queue
 */
int queue_push(void *item);

/**
 * @brief Get the first item in the queue
 *
 * @return The number of items in the queue
 */
int queue_peek(void *item);

/**
 * @brief Drop the first item in the queue
 *
 * @return The number of items in the queue
 */
int queue_drop(void);

#ifdef __cplusplus
}
#endif

#endif
