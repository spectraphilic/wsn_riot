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
 * @brief Send an SDI-12 command and return answer
 */
int wsn_sdi_command(const char* cmd, char out[]);

/**
 * @brief Initialize the sensors
 */
int wsn_sensors_init(void);


#ifdef __cplusplus
}
#endif

#endif
