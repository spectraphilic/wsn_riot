// RIOT
#include <log.h>

// Project
#include "settings.h"
#include "wsn.h"


void wsn_boot(void)
{
#ifdef MODULE_FATFS_VFS
    if (wsn_mount() == 0) {
#ifdef MODULE_SETTINGS
        settings_load();
        LOG_INFO("Settings loaded\n");
#else
        LOG_WARNING("Missing settings module\n");
#endif
    }
#else
    LOG_WARNING("Missing VFS module\n");
#endif
}
