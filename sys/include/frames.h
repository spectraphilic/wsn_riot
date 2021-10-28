/**
 * @defgroup wsn Wireless Sensor Network
 * @brief Frames storage in the filesystem
 *
 * TODO Write longer description
 *
 * @{
 *
 * @file
 */

#ifndef FRAMES_H
#define FRAMES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the frames layout in the filesystem
 */
int frames_init(void);

/**
 * @brief Saves a frame into the filesystem
 */
int frames_save(time_t time, uint8_t *data, uint8_t length);

/**
 * @brief Loads a frame from the filesystem
 */
int frames_load(uint8_t *data, uint8_t *size);

/**
 * @brief Drop the first frame from the filesystem
 */
int frames_drop(void);

#ifdef __cplusplus
}
#endif

#endif

