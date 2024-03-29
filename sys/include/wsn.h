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

// Riot
#include <net/gnrc/netif.h>
#include <timex.h>
#include <ztimer.h>

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
 * @brief Prints the PM blockers, for debugging only.
 */
void wsn_debug_pm(void);

/**
 * @brief Format SD card
 */
int wsn_format(void);

/**
 * @brief Mount filesystem
 *
 * Mounts the FAT filesystem in SD card at the root directory.
 */
int wsn_mount(void);

/**
 * @brief Initialize the network
 */
int wsn_network_get_opt(gnrc_netif_t *gnrc_netif, netopt_t opt, void *value, size_t max_len);

/**
 * @brief Initialize the network
 */
int wsn_network_init(void);

/**
 * @brief Send data to the network
 */
int wsn_network_send(const uint8_t *data, size_t size);

/**
 * @brief Initialize the external RTC
 */
int wsn_rtc_init(void);

/**
 * @brief Return the temperature read from the RTC
 */
int wsn_rtc_temp(int16_t *temp);

/**
 * @brief Return the time read from the RTC
 */
int wsn_rtc_time_get(struct tm *time);

/**
 * @brief Set RTC time with the given time
 */
int wsn_rtc_time_set(struct tm *time);

/**
 * @brief Trigger alarm after the given delay (in seconds)
 */
int wsn_rtc_alarm_set(struct tm *time);

/**
 * @brief Returns the basetime
 *
 * The basetime is an internal variable, useful to calculate the system time.
 * If the value is zero it means that the system time has not been set yet.
 */
ztimer_now_t wsn_time_basetime(void);

/**
 * @brief Returns system time
 */
ztimer_now_t wsn_time_get(unsigned *ms);

/**
 * @brief Load system time from RTC
 */
int wsn_time_load(void);

/**
 * @brief Sets system time
 */
int wsn_time_set(ztimer_now_t time);

#ifdef __cplusplus
}
#endif

#endif
