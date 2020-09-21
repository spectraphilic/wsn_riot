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

int settings_save(void);
int settings_load(void);

extern struct Settings settings;

#endif
