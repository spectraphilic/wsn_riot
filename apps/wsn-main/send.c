#include <stdio.h>

// Riot
#include <log.h>
#include <net/gnrc/netif.h>
#include <net/gnrc/netif/hdr.h>


int send_data(const uint8_t *data, size_t size)
{
    gnrc_pktsnip_t *pkt, *hdr;
    gnrc_netif_hdr_t *nethdr;

    netif_t *netif = netif_get_by_id(2);

    // Broadcast
    uint8_t addr[GNRC_NETIF_L2ADDR_MAXLEN];
    size_t addr_len = 0;
    uint8_t flags = 0x00;
    if (netif) {
        flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    }

    // Data
    LOG_INFO("gnrc_pktbuf_add(...) size = %d", size);
    pkt = gnrc_pktbuf_add(NULL, data, size, GNRC_NETTYPE_UNDEF);
    if (pkt == NULL) {
        LOG_ERROR("Unable to copy data to packet buffer");
        return -1;
    }

    // Header
    hdr = gnrc_netif_hdr_build(NULL, 0, addr, addr_len);
    if (hdr == NULL) {
        LOG_ERROR("Packet buffer full");
        gnrc_pktbuf_release(pkt);
        return -1;
    }
    LL_PREPEND(pkt, hdr);
    nethdr = (gnrc_netif_hdr_t *)hdr->data;
    nethdr->flags = flags;

    // Send
    if (gnrc_netif_send((gnrc_netif_t *)netif, pkt) < 1) {
        LOG_ERROR("Unable to send");
        gnrc_pktbuf_release(pkt);
        return -1;
    }

    return 0;
}
