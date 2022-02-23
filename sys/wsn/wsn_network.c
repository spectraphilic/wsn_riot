#include <stddef.h>

// Riot
#include <fmt.h>
#include <log.h>
#include <net/gnrc/netif.h>
#include <net/gnrc/netif/hdr.h>


static gnrc_netif_t* get_gnrc_netif(void)
{
    gnrc_netif_t *netif = NULL;

    // Get the first network interface (reliable if there's only one)
    // XXX Use netif_get_by_name, gnrc_netif_get_by_pid, something else?
    // With remote-revb board the netif id will be 2
    // With feather-m0 board the netif id will be 3
    netif = gnrc_netif_iter(netif);
    if (netif == NULL) {
        LOG_ERROR("Network interface not found");
    }

    return netif;
}

#if IS_USED(MODULE_SX127X)
static uint8_t address = 0;
#endif

static int wsn_network_set_opt(netif_t *netif, netopt_t opt, void *value, size_t value_len)
{
#if IS_USED(MODULE_SX127X)
    switch (opt) {
        case NETOPT_ADDRESS:
            address = *(uint8_t*)value;
            return 1;
        default:
            break;
    }
#endif

    return netif_set_opt(netif, opt, 0, value, value_len);
}

int wsn_network_get_opt(gnrc_netif_t *gnrc_netif, netopt_t opt, void *value, size_t max_len)
{
    if (gnrc_netif == NULL) {
        gnrc_netif = get_gnrc_netif();
    }

#if IS_USED(MODULE_SX127X)
    switch (opt) {
        case NETOPT_ADDRESS:
            *(uint8_t*)value = address;
            return 1;
        default:
            break;
    }
#endif

    netif_t *netif = (netif_t*)gnrc_netif;
    return netif_get_opt(netif, opt, 0, value, max_len);
}


static int wsn_network_listen(gnrc_netif_t *gnrc_netif)
{
    // Adapted from RIOT/tests/driver_sx127x/main.c
    netif_t *netif = (netif_t*)gnrc_netif;

    // Switch to continuous listen mode
    netopt_enable_t single = false;  // XXX const?
    netif_set_opt(netif, NETOPT_SINGLE_RECEIVE, 0, &single, sizeof(single));

    uint32_t timeout = 0;  // XXX const?
    netif_set_opt(netif, NETOPT_RX_TIMEOUT, 0, &timeout, sizeof(timeout));

    // Switch to RX state
    netopt_state_t state = NETOPT_STATE_RX;
    netif_set_opt(netif, NETOPT_STATE, 0, &state, sizeof(state));

    return 0;
}


int wsn_network_init(void)
{
    /*
     * Network
     * https://riot-os.org/api/group__net__netopt.html
     */
    int rc;

    gnrc_netif_t *gnrc_netif = get_gnrc_netif();
    if (gnrc_netif == NULL) {
        return -1;
    }

    netif_t *netif = (netif_t*)gnrc_netif;

    // Set configuration
    uint8_t address[GNRC_NETIF_L2ADDR_MAXLEN];
#if IS_USED(MODULE_SX127X)
    address[0] = 0x02;  // FIXME hardcoded
    rc = wsn_network_set_opt(netif, NETOPT_ADDRESS, &address, sizeof(address));
#else
    uint16_t channel = 0x17, nid = 0x4B5;
    rc = wsn_network_set_opt(netif, NETOPT_CHANNEL, &channel, sizeof(channel));
    rc = wsn_network_set_opt(netif, NETOPT_NID, &nid, sizeof(nid));
#endif

    // Address (short)
    char address_str[6] = "?";
    rc = wsn_network_get_opt(gnrc_netif, NETOPT_ADDRESS, &address, sizeof(address));
    if (rc > 0) {
        gnrc_netif_addr_to_str(address, rc, address_str);
    }

    // Interface id
    int16_t id = netif_get_id(netif);

#if IS_USED(MODULE_SX127X)
    LOG_INFO("netif id=%d addr=%s", id, address_str);
#else
    // Channel
    char channel_str[5];
    rc = wsn_network_get_opt(gnrc_netif, NETOPT_CHANNEL, &channel, sizeof(channel));
    if (rc > 0) {
        rc = fmt_u16_hex(channel_str, channel);
        channel_str[rc] = '\0';
    }

    // Networkd id
    char nid_str[5];
    rc = wsn_network_get_opt(gnrc_netif, NETOPT_NID, &nid, sizeof(nid));
    if (rc > 0) {
        rc = fmt_u16_hex(nid_str, nid);
        nid_str[rc] = '\0';
    }

    // Print information
    LOG_INFO("netif id=%d addr=%s chan=%s nid=%s", id, address_str, channel_str, nid_str);
#endif

    // Listen the network
    wsn_network_listen(gnrc_netif);

    return 0;
}


int wsn_network_send(const uint8_t *data, size_t size)
{
    gnrc_pktsnip_t *pkt, *hdr;
    gnrc_netif_hdr_t *nethdr;
    int rc = 0;

    gnrc_netif_t* netif = get_gnrc_netif();
    if (netif == NULL) {
        return -1;
    }

    // Broadcast
    uint8_t addr[GNRC_NETIF_L2ADDR_MAXLEN];
    size_t addr_len = 0;
    uint8_t flags = 0x00;
    if (netif) {
        flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    }

    // Data
    //LOG_INFO("gnrc_pktbuf_add(...) size = %d", size);
    pkt = gnrc_pktbuf_add(NULL, data, size, GNRC_NETTYPE_UNDEF);
    if (pkt == NULL) {
        LOG_ERROR("Unable to copy data to packet buffer");
        rc = -1;
        goto exit;
    }

    // Header
    hdr = gnrc_netif_hdr_build(NULL, 0, addr, addr_len);
    if (hdr == NULL) {
        LOG_ERROR("Packet buffer full");
        gnrc_pktbuf_release(pkt);
        rc = -1;
        goto exit;
    }
    LL_PREPEND(pkt, hdr);
    nethdr = (gnrc_netif_hdr_t *)hdr->data;
    nethdr->flags = flags;

    // Send
    int error = gnrc_netif_send(netif, pkt);
    if (error < 1) {
        LOG_ERROR("Unable to send error=%d", error);
        gnrc_pktbuf_release(pkt);
        rc = -1;
    }

exit:
    // Listen the network
    wsn_network_listen(netif);

    return rc;
}
