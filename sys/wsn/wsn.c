// RIOT
#include <log.h>
#include <net/netif.h>
#include <net/gnrc/netif.h>
#include <periph/cpuid.h>

// Project
#include "settings.h"
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

    /*
     * Storage
     */
#ifdef MODULE_FATFS_VFS
    if (wsn_mount() == 0) {
        #ifdef MODULE_SETTINGS
            if (settings_load() == 0) {
                LOG_INFO("Settings loaded\n");
            }
        #else
            LOG_WARNING("Missing settings module\n");
        #endif
    }
#else
    LOG_WARNING("Missing VFS module\n");
#endif

#ifdef MODULE_GNRC_NETIF
    /*
     * Network
     * https://riot-os.org/api/group__net__netopt.html
     */
    int res;
    int16_t id;
    uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
    uint16_t channel = 0x17, nid = 0x4B5;
    char channel_str[5];
    char nid_str[5];
    char hwaddr_str[24];

    netif_t *netif = NULL;
    while ((netif = netif_iter(netif))) {
        // Set configuration
        res = netif_set_opt(netif, NETOPT_CHANNEL, 0, &channel, sizeof(channel));
        res = netif_set_opt(netif, NETOPT_NID, 0, &nid, sizeof(nid));

        // Print information
        id = netif_get_id(netif);

        res = netif_get_opt(netif, NETOPT_ADDRESS, 0, hwaddr, sizeof(hwaddr));
        if (res >= 0) {
            gnrc_netif_addr_to_str(hwaddr, res, hwaddr_str);
        }

        res = netif_get_opt(netif, NETOPT_CHANNEL, 0, &channel, sizeof(channel));
        if (res >= 0) {
            res = fmt_u16_hex(channel_str, channel);
            channel_str[res] = '\0';
        }

        res = netif_get_opt(netif, NETOPT_NID, 0, &nid, sizeof(nid));
        if (res >= 0) {
            res = fmt_u16_hex(nid_str, nid);
            nid_str[res] = '\0';
        }

        LOG_INFO("NETWORK id=%d addr=%s chan=%s nid=%s\n", id, hwaddr_str, channel_str, nid_str);
    }
#endif
}
