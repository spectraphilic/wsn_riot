#ifndef SETTINGS_H
#define SETTINGS_H

typedef enum {
    WAN_DISABLED,
    WAN_4G,
    WAN_IRIDIUM,
    WAN_LEN
} wan_type_t;

struct Settings {
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
