#include <assert.h>

// RIOT
#include <board.h>
#include <log.h>
#include <nanocbor/nanocbor.h>
#include <net/netif.h>
#include <net/gnrc/netif.h>
#include "net/gnrc/netif/hdr.h"
#include <saul_reg.h>
#include <timex.h>
#include <ztimer.h>

// Project
#include "triage.h"
#include "wsn.h"


#ifdef MODULE_ZTIMER_MSEC
#define ZTIMER ZTIMER_MSEC
#define TICKS_PER_SEC MS_PER_SEC
#else
#define ZTIMER ZTIMER_USEC
#define TICKS_PER_SEC US_PER_SEC
#endif

#define SLEEP 30 // seconds


static int send(uint8_t *data, size_t size) {
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
    pkt = gnrc_pktbuf_add(NULL, data, size, GNRC_NETTYPE_UNDEF);
    if (pkt == NULL) {
        LOG_ERROR("unable to copy data to packet buffer\n");
        return -1;
    }

    // Header
    hdr = gnrc_netif_hdr_build(NULL, 0, addr, addr_len);
    if (hdr == NULL) {
        LOG_ERROR("packet buffer full\n");
        gnrc_pktbuf_release(pkt);
        return -1;
    }
    LL_PREPEND(pkt, hdr);
    nethdr = (gnrc_netif_hdr_t *)hdr->data;
    nethdr->flags = flags;

    // Send
    if (gnrc_netif_send((gnrc_netif_t *)netif, pkt) < 1) {
        LOG_ERROR("unable to send");
        gnrc_pktbuf_release(pkt);
        return -1;
    }

    return 0;
}



int main(void)
{
    uint8_t buffer[128];
    nanocbor_encoder_t enc;
    phydat_t res;

    LED0_ON;
    //test_utils_interactive_sync();

    // Boot
    wsn_boot();
    LOG_INFO("app=wsn-main board=%s mcu=%s\n", RIOT_BOARD, RIOT_MCU);
    LOG_INFO("This program loops forever, sleeping for %ds in every loop.\n", SLEEP);

    // Main loop
    for (unsigned int loop=0; ; loop++) {
        LOG_INFO("Loop=%u\n", loop);

        // Read sensors and fill buffer
        nanocbor_encoder_init(&enc, buffer, sizeof(buffer));
        nanocbor_fmt_array_indefinite(&enc);

        saul_reg_t *dev = saul_reg;
        int i = 0;
        while (dev) {
            printf("%02i: %s\t%s ", i, dev->name, saul_class_to_str(dev->driver->type));
            int dim = saul_reg_read(dev, &res);
            if (dim <= 0) {
                printf("ERROR\n");
            } else {
                nanocbor_fmt_uint(&enc, i);
                for (int j=0; j < dim; j++) {
                    int value = res.val[j];
                    printf("%d ", value);
                    nanocbor_fmt_int(&enc, value);
                }
                printf("unit=%s scale=%d\n", phydat_unit_to_str(res.unit), res.scale);
            }

            // Next
            dev = dev->next;
            i++;
        }

        nanocbor_fmt_end_indefinite(&enc);
        size_t required = nanocbor_encoded_len(&enc);
        assert(required);

        printf("CBOR = ");
        for (size_t k=0; k < required; k++) {
            printf("%02x", buffer[k]);
        }
        printf("\n");

        // Send
        send(buffer, required);

        // Done
        LOG_INFO("Loop=%u DONE\n", loop);
        LED0_OFF;
        ztimer_sleep(ZTIMER, SLEEP * TICKS_PER_SEC);
        LED0_ON;
    }

    return 0;
}
