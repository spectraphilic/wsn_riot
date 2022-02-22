#include <stddef.h>

// Riot
#include <fmt.h>
#include <log.h>
#include <net/netif.h>
#include <net/gnrc/netif.h>
#include <net/gnrc/netif/hdr.h>


static netif_t* get_netif(void)
{
    netif_t *netif = NULL;

    // Get the first network interface (reliable if there's only one)
    // XXX Use netif_get_by_name or something else?
    // With remote-revb board the netif id will be 2
    // With feather-m0 board the netif id will be 3
    netif = netif_iter(netif);
    if (netif == NULL) {
        LOG_ERROR("Network interface not found");
    }

    return netif;
}


static int wsn_network_listen(netif_t *netif)
{
    // XXX Adapted from RIOT/tests/driver_sx127x/main.c

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
    int res;
    int16_t id;
    uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
    uint16_t channel = 0x17, nid = 0x4B5;
    char channel_str[5];
    char nid_str[5];
    char hwaddr_short[6];
    char hwaddr_long[24];

    netif_t *netif = get_netif();
    if (netif == NULL) {
        return -1;
    }

    // Set configuration
    res = netif_set_opt(netif, NETOPT_CHANNEL, 0, &channel, sizeof(channel));
    res = netif_set_opt(netif, NETOPT_NID, 0, &nid, sizeof(nid));

    // Print information
    id = netif_get_id(netif);

    res = netif_get_opt(netif, NETOPT_ADDRESS, 0, hwaddr, sizeof(hwaddr));
    if (res >= 0) {
        gnrc_netif_addr_to_str(hwaddr, res, hwaddr_short);
    }

    res = netif_get_opt(netif, NETOPT_ADDRESS_LONG, 0, hwaddr, sizeof(hwaddr));
    if (res >= 0) {
        gnrc_netif_addr_to_str(hwaddr, res, hwaddr_long);
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

    LOG_INFO(
        "netif id=%d addr_long=%s addr_short=%s chan=%s nid=%s",
        id,
        hwaddr_long,
        hwaddr_short,
        channel_str,
        nid_str
    );

    // Listen the network
    wsn_network_listen(netif);

    return 0;
}


int wsn_network_send(const uint8_t *data, size_t size)
{
    gnrc_pktsnip_t *pkt, *hdr;
    gnrc_netif_hdr_t *nethdr;
    int rc = 0;

    netif_t* netif = get_netif();
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
    int error = gnrc_netif_send((gnrc_netif_t *)netif, pkt);
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
