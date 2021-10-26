/**
 * @defgroup wsn Wireless Sensor Network
 * @brief Common code to support the WSN data logger
 *
 * In this module we temporarily keep helper functions, until we find a better
 * place for each of them.
 *
 * @{
 *
 * @file
 */

#ifndef WSN_H
#define WSN_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Uniquely identifies the mote
 */
extern uint64_t cpuid;

/**
 * @brief Code to run at boot
 *
 * Mounts the FAT filesystem in SD card at the root directory. Loads the
 * dynamic configuration from the settings file.
 */
void wsn_boot(void);

/**
 * @brief Mount filesystem
 *
 * Mounts the FAT filesystem in SD card at the root directory.
 */
int wsn_mount(void);

/**
 * @brief Initialize the network
 */
int wsn_network_init(void);

/**
 * @brief Saves a frame into the filesystem
 */
int wsn_save_frame(time_t time, uint8_t *data, uint8_t length);

/**
 * @brief Loads a frame from the filesystem
 */
int wsn_load_frame(uint8_t *data, uint8_t *size);

/**
 * @brief Drop the first frame from the filesystem
 */
int wsn_drop_frame(void);

/**
 * @brief Returns the basetime
 *
 * The basetime is an internal variable, useful to calculate the system time.
 * If the value is zero it means that the system time has not been set yet.
 */
time_t wsn_time_basetime(void);

/**
 * @brief Returns system time
 */
time_t wsn_time_get(void);

/**
 * @brief Sets system time
 */
int wsn_time_set(time_t time);


#ifdef __cplusplus
}
#endif

#endif
