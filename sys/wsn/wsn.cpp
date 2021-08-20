// RIOT
#include <log.h>
#include <periph/cpuid.h>

// Project
#include "settings.h"
#include "qtpy.h"
#include "wsn.h"


uint64_t cpuid = 0;


void wsn_boot(void)
{
    /*
     * ID
     *
     * XXX In the cc2539 the CPU id is based on the MAC address, which is
     * defined by two 32bit little-endian words stored in memory: first the
     * most-significat word (IEEE_ADDR_MSWORD), then the less-significat word
     * (IEEE_ADDR_MSWORD). But the CPU id is read from memory as-is.
     *
     * For example if in memory we have 00:4b:12:00 2e:15:40:19 then:
     * - MAC address is 00:12:4B:00:19:40:15:2E
     * - But CPU id is  00:4b:12:00:2e:15:40:19
     *
     */
    uint8_t buffer[CPUID_LEN];
    cpuid_get(buffer);

    for (unsigned int i = 0; i < CPUID_LEN; i++) {
        cpuid = cpuid << 8;
        cpuid |= buffer[i];
    }

    // Storage
    if (IS_USED(MODULE_FATFS_VFS)) {
        if (wsn_mount() == 0) {
            if (IS_USED(MODULE_SETTNGS)) {
                if (settings_load() == 0)
                    LOG_INFO("Settings loaded\n");
            } else {
                LOG_WARNING("Missing settings module\n");
            }
        }
    } else {
        LOG_WARNING("Missing VFS module\n");
    }

    // Sensors
    if (IS_USED(MODULE_QTPY))
        qtpy_init_auto();

    // Network
    if (IS_USED(MODULE_GNRC_NETIF))
        wsn_network_init();
}