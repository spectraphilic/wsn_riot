#ifndef SETTINGS_H
#define SETTINGS_H

typedef enum {
    LOG_OFF,
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_TRACE,
    LOG_LEN // Special value
} log_level_t;

typedef enum {
    WAN_DISABLED,
    WAN_4G,
    WAN_IRIDIUM,
    WAN_LEN
} wan_type_t;

struct Settings {
    log_level_t log_level;
    wan_type_t wan_type;
};

int settings_index(const char *name);
int settings_set(const char *name, const char *value);
int settings_save(void);
int settings_load(void);

extern struct Settings settings;
extern const char * const settings_names[];
extern const size_t settings_len;

#endif
