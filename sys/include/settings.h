/**
 * @defgroup settings Settings
 * @brief Manage dynamic configuration in the /settings.txt file
 *
 * Where configuration is a pre-defined set of variables.
 *
 * @{
 */

#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * @brief Defines the Wide Area Network used
 */
typedef enum {
    WAN_DISABLED, ///< No wide area network
    WAN_4G, ///< 4G network
    WAN_IRIDIUM, ///< Iridium satellite network
    WAN_LEN
} wan_type_t;

/**
 * @brief Configuration variables in memory
 */
struct Settings {
    wan_type_t wan_type; ///< Wide Area Network type
};

/**
 * @brief Return the index of a configuration variable
 *
 * The index maps the variable name to a number, from zero to the number of
 * variables minus one.
 *
 * This can be used to associate information to a variable, using an array.
 */
int settings_index(const char *name);

/**
 * @brief Return the name of the variable for the given index number
 */
const char *settings_name(size_t index);

/**
 * @brief Set configuration variable
 *
 * Where the value is given as a text string. The value will be parsed and
 * stored in memory, but it won't be saved to disk. Use settings_save to
 * make the change persistent.
 *
 * It also possible to simply set the value directly in the global settings,
 * and then call settings_save.
 */
int settings_set(const char *name, const char *value);

/**
 * @brief Save configuration to file
 */
int settings_save(void);

/**
 * @brief Load configuration from file
 */
int settings_load(void);


/**
 * @brief Global that keeps the configuration in memory
 *
 * Use it directrly for reading or writing:
 *
 *     if (settings.wan_type == WAN_4G) {
 *
 * Or writing (remember to save the changes):
 *
 *     settings.wan_type = WAN_IRIDIUM;
 *     settings_save();
 */
extern struct Settings settings;

/**
 * @brief Number of configuration variables
 */
extern const size_t settings_len;

#endif
/** @} */
