// RIOT
#include <log.h>
#include <net/netif.h>
#include <net/gnrc/netif.h>

// Project
#include "settings.h"
#include "wsn.h"


void wsn_boot(void)
{
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
}
